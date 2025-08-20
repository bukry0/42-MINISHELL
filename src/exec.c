/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:02:52 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:02:52 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
Executor nedir?
Shell yazarken “executor” dediğimiz şey, parser’dan gelen hazır komut listesini alır.
Bu komutları gerçekten işletim sistemine çalıştırır. Kısacası:

Parser → sadece “ne istendiğini” çözümler (ls -l | grep foo gibi).
Executor → “nasıl çalıştırılacağını” halleder (pipe aç, fork yap, exec çağır, built-in çalıştır vs.).
Yani executor, shell’in komut motoru.

Built-in nedir?
Built-in, shell’in kendi içinde tanımlı komutlarıdır. Mesela:

cd → dizin değiştirme
echo → ekrana yazı basma
export → environment değişkeni ayarlama
exit → shell’i kapatma

Bunlar “dış programlar” değildir. Çünkü cd gibi bir şey child process’te çalıştırılsa hiçbir işe yaramaz.
Örneğin fork yapıp child process’te cd çalıştırsan, parent shell’in dizini değişmez.
Bu yüzden built-in komutlar fork’suz, direkt shell içinde çalıştırılır.

External program nedir?
Shell’in kendi built-in’i değilse → o zaman bir program dosyası çağırılır. Örneğin;

ls, grep, cat, vim …
Bunlar /bin/ls, /usr/bin/grep gibi dosyalardır.

Bu tür komutlar için:
fork yaparız → yeni bir process oluşur.
Child process içinde exec ile ls programı çalıştırılır.
Parent shell bekler (waitpid).
Neden? Çünkü parent shell’in kendisini ls’ye dönüştürürsek artık shell olmaz :).

Fork nedir?
fork() → yeni bir process (child) oluşturur.
Child, parent’ın kopyasıdır.
Kod akışı hem parent’ta hem child’da devam eder.
fork() return value ile hangisinin parent, hangisinin child olduğunu ayırt ederiz.

Exec nedir?
Child process oluştu → ama o sadece shell’in kopyası.
Biz o child’ı başka bir programla değiştirmek isteriz.
execve(path, argv, env) çağrıldığında:

O process’in memory’si tamamen silinir.
Yerine mesela /bin/ls yüklenir.
Yani child process artık shell kodu değil, ls’in kodu çalıştırır.
Parent shell bu sırada kendi yaşamına devam eder ve child’ın bitmesini wait ile bekler.

Neden bazen fork kullanmıyoruz?
Built-in komutlarda (örneğin cd) → parent process’te direkt çalışmalı. Çünkü parent’ın durumunu değiştirmesi gerekir.
External komutlarda (ls, grep) → fork + exec yaparız. Çünkü onlar ayrı programlar. Yani;

Built-in → fork yok
External → fork + exec

Pipeline (|) olduğunda ne oluyor? Diyelim;

ls | grep foo

Adımlar:
Pipe açılır → bize 2 fd verilir: pipe[0] (read), pipe[1] (write).
ls child process’te çalıştırılır, stdout → pipe[1]’e yönlendirilir.
grep foo başka bir child process’te çalıştırılır, stdin → pipe[0]’a yönlendirilir.
Parent shell fd’leri kapatır ve çocukların bitmesini bekler.
Böylece ls çıktısı grep’e akar.

Executor’un görevi özetle:
Komut built-in mi? → parent içinde çalıştır.
Yoksa external mi? → fork + exec yap.
Pipe/redirection varsa → fd’leri ayarla, child’lara aktar.
Parent → fd’leri kapat, çocukları bekle, exit status kaydet.
Yani execute_cmds fonksiyonun arkasındaki fikir bu;
Tek komut + built-in → direkt çalıştır.
Diğer durumlar (external, pipe) → fork + exec ile çalıştır.

Adım 1: En basit shell (tek komut, sadece external)
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <sys/wait.h>

	int main()
	{
    	char *argv[] = {"ls", "-l", NULL}; // Çalıştırmak istediğimiz program
	    pid_t pid = fork();                // 1. Çocuk yarat

	    if (pid == 0) // child process
	    {
	        execvp(argv[0], argv); // 2. Çocuğu "ls -l" programına dönüştür
	        perror("exec failed");
    	    exit(1);
	    }
    	else // parent process
	    {
	        wait(NULL); // 3. Çocuğun bitmesini bekle
	        printf("child bitti\n");
	    }
	    return 0;
	}
Burada:
fork → çocuk yaratıyor.
execvp → çocuğu ls -l programına dönüştürüyor.
wait → parent çocuğun bitmesini bekliyor.
Bu en temel executor mantığıdır.

Adım 2: Kullanıcıdan komut alma
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <sys/wait.h>

	int main()
	{
    	char input[100];

	    while (1)
    	{
	        printf("myshell> ");
	        fgets(input, sizeof(input), stdin);
	        input[strcspn(input, "\n")] = 0; // newline sil

	        if (strcmp(input, "exit") == 0) // çıkış
	            break;

	        pid_t pid = fork();
	        if (pid == 0)
	        {
	            char *argv[] = {input, NULL}; // sadece tek kelime komut (örn: "ls")
	            execvp(argv[0], argv);
	            perror("exec failed");
	            exit(1);
	        }
	        else
	        {
	            wait(NULL);
	        }
	    }
	    return 0;
	}

Artık kendi yazdığın shell’de ls, pwd, date gibi programları çağırabilirsin.

Adım 3: Built-in ekleme

Diyelim ki cd ekleyelim.

	if (strncmp(input, "cd ", 3) == 0)
	{
		chdir(input + 3); // "cd " sonrası path'e git
	    continue;
	}


Burada fork kullanmıyoruz çünkü dizini değiştirmek parent shell’in kendisinde olmalı.
İşte bu yüzden built-in komutlar farklıdır.

Adım 4: Pipe ekleme (|)
ls | grep foo

Bunu yapmak için:

Pipe açılır (pipe(p) → p[0], p[1])
Bir child yaratılır → stdout → p[1] yönlendirilir
Başka bir child yaratılır → stdin → p[0] yönlendirilir
Parent fd’leri kapatır, wait eder

Adım 5: Redirection (<, >, >>)
Aynı mantık:
> → stdout’u dosyaya yönlendir (dup2(fd, STDOUT_FILENO))
< → stdin’i dosyadan oku (dup2(fd, STDIN_FILENO))

Özet:
Executor → parser’dan gelen komutları alıp çalıştıran kısım.
Built-in → shell’in kendi fonksiyonları (fork yok).
External → ayrı program dosyaları (fork + exec).
Pipe/redirect → exec’ten önce fd yönlendirme yapılır.

Built-in Nedir?
Bir built-in, shell’in kendi içinde yazılmış fonksiyonudur.
cd → başka bir program değil, shell’in kendisinin dizinini değiştirir.
exit → shell’i kapatır (başka program değil).
export → bir environment değişkeni ekler.
echo → terminale yazar.

Bunlar dışarıda /bin/cd gibi ayrı bir executable yoktur.
Shell bunları kendi içinde halletmek zorunda.

Neden Fork Kullanmıyoruz? Düşün:

cd /home
Eğer fork yapıp çocuğa chdir("/home") yaptırsan → sadece çocuğun working directory’si değişir.
Parent (shell) yine eski dizinde kalır.
Ama bizim istediğimiz shell’in kendisinin dizin değiştirmesi. Yani parent değişmeli.
Bu yüzden cd → direkt fonksiyon olarak çalışmalı, fork yok.
Aynı mantık exit için de geçerli:
Eğer fork yapıp child’da exit() çağırsan → sadece child kapanır, shell devam eder.
Ama sen shell’i kapatmak istiyorsun. Yani parent’ta exit(0) çağrılmalı.

Somutlaştıralım:
External (fork + exec gerekir)

myshell> ls

Shell: fork → child yaratır.
Child: exec(“/bin/ls”) → kendini ls programına dönüştürür.
Parent: wait() → bekler.
Çünkü ls zaten /bin/ls diye bir dosya. Shell sadece onu çağırır.

Built-in (fork gerekmez)

myshell> cd ..

Shell: kendisi chdir("..") çağırır.
Child gerekmez. Örnek Kod; Küçük Shell
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <sys/wait.h>

	int main()
	{
    	char input[100];

	    while (1)
	    {
	        printf("myshell> ");
	        fgets(input, sizeof(input), stdin);
	        input[strcspn(input, "\n")] = 0;

	        // exit
	        if (strcmp(input, "exit") == 0)
	            break;

	        // cd
	        if (strncmp(input, "cd ", 3) == 0)
	        {
	            if (chdir(input + 3) != 0)
	                perror("cd failed");
	            continue;
	        }

	        // external program
	        pid_t pid = fork();
	        if (pid == 0)
	        {
	            char *argv[] = {input, NULL};
	            execvp(argv[0], argv);
	            perror("exec failed");
	            exit(1);
	        }
	        else
	            wait(NULL);
	    }
	    return 0;
	}

Burada:
cd ve exit built-in. Direkt çalıştırılıyor.
Diğer her şey fork + exec ile çalışıyor.

Küçük Deneme Senaryosu
cd /tmp → dizin değişmeli (fork yok).
ls → external program çalışmalı (fork + exec).
pwd (built-in olsa → kendimiz print working directory yaparız, external olsa /bin/pwd).
exit → shell’i kapatır.

Genel Mantığı Özetlersek:

Built-in olmalı çünkü:
Shell’in kendisini etkilemesi gerekiyor (örn: cd, exit, export).
Ya da çok basit/performans açısından kritik (örn: echo, pwd).

External olmalı çünkü:
Shell’in kendi içinde yazılmasına gerek yok.
Daha karmaşık işler (örn: ls, grep, vim) → ayrı program zaten var.

O yüzden:
cd, exit → zorunlu olarak built-in.
echo, pwd, export, unset → performans/kolaylık için built-in.
ls, grep, cat → external.

Dosya Sistemi Tarafı:
Linux’ta /bin dizini → “binary executables” (çalıştırılabilir dosyalar) tutulur.
Bu dosyalar genelde makine kodu içeren programlardır. Yani gcc ile derlenmiş, senin C kodunun ürettiği .exe gibi.
Mesela /bin/ls gerçekten tek başına derlenmiş bir programdır. İçinde:
main() fonksiyonu,
dizin okuma (opendir, readdir),
çıktı yazma (printf gibi) fonksiyonları çağırılır.
Sen ls yazınca shell → fork + execve("/bin/ls") yapar. Yani o dosyayı RAM’e yükler, çalıştırır.

Sen bugüne kadar hep fonksiyon çağırmaya alışmışsın (exit(), printf(), malloc() vs).
Bunlar senin programının içinde derlenmiş, çalıştığın dilin (C) sağladığı fonksiyonlar.
Yani derleme zamanında kodunun bir parçası haline gelir.
Ama terminalde ls yazdığında aslında bir başka program çalıştırıyorsun.
O program, senin programının (örneğin bash) içinde değil.
Dosya sisteminde, diskte duruyor: /bin/ls.

Fonksiyon vs. Program:
exit() → senin programının içindeki hazır bir fonksiyon.
ls → ayrı derlenmiş bir program dosyası. Senin programının parçası değil, disk üzerinde duruyor.

Sen ls yazınca:
Shell bakar PATH’e (/bin içinde bulur).
O dosya çalıştırılır → sanki sen ./a.out çalıştırmışsın gibi.
Shell sadece bekler, iş bitince kontrol geri gelir.
Küçük bir örnek:

which ls

yazarsan → sana ls komutunun hangi dosya olduğunu gösterir. Genelde /bin/ls.
cat /bin/ls dersen → göreceğin şey karmaşık semboller olur çünkü o dosya makine kodu (binary). O yüzden doğrudan okunmaz.

Özet:
Shell içindeki komutlar → fonksiyon gibi (built-in).
/bin/ls gibi şeyler → başka programlar, ayrı dosyalar. Shell sadece onları çağırıyor.

*/

/*
1. fork(): “Yeni bir süreç (process) yaratır.”
Shell kendisi zaten çalışan bir programdır.
Bir komutu çalıştırmak istediğinde yeni bir süreç açar (çocuğunu yaratır).
Bu sayede shell kendi hayatına devam ederken, komutu farklı bir yerde çalıştırabilir.

	#include <unistd.h>
	#include <stdio.h>

	int main(void)
	{
    	int pid = fork(); // yeni bir process yarat

	    if (pid == 0) // çocuk süreç buraya girer
	        printf("Ben çocuk süreçim!\n");
	    else // ebeveyn süreç buraya girer
	        printf("Ben ebeveynim, çocuğun pid'si: %d\n", pid);

    	return 0;
	}

Çalıştırınca 2 çıktı alırsın çünkü artık 2 süreç var. Shell de benzer şekilde çalışır: her komut için fork ile yeni bir süreç açar.

2. execve(): “Var olan süreci başka bir programla değiştirir.”
fork ile bir çocuk süreç yarattın. Ama bu çocuk şu anda senin programının kopyası.
Ona diyorsun ki: “Git, ls programını çalıştır.”
Bunu yapmak için execve() kullanılır.

	#include <unistd.h>

	int main(void)
	{
	    char *args[] = {"/bin/ls", "-l", NULL}; 
	    execve("/bin/ls", args, NULL);
	    return 0; // buraya asla gelmez!
	}

Burada process vardı ama execve ile içi tamamen /bin/ls programı ile değişti. Artık o process senin programın değil, ls oldu.

3. wait() / waitpid(): “Çocuğunun işini bitirmesini bekle.”
Ebeveyn fork yaptı → çocuk çalışmaya başladı.
Eğer ebeveyn hemen devam ederse, çocukla senkronize olamaz.
wait() çağırırsan, shell bekler ve çocuk bitince çıkış kodunu alır.

	#include <sys/wait.h>
	#include <unistd.h>
	#include <stdio.h>

	int main(void)
	{
	    int pid = fork();

	    if (pid == 0)
	        execlp("ls", "ls", "-l", NULL); // çocuk: ls çalıştır
	    else
		{
	        int status;
	        wait(&status); // ebeveyn: çocuk bitene kadar bekle
	        printf("Çocuk bitti, çıkış kodu: %d\n", WEXITSTATUS(status));
	    }
	}

Böylece shell, ls komutu bittiğinde sana döner ve $? gibi şeylerle çıkış kodunu öğrenir.

Neden bazılarında kullanmıyoruz?
Built-in komutlar (cd, exit, echo): Shell’in kendi içinde yapılır. fork/exec gerekmez.
Harici komutlar (ls, grep, cat): Bunlar diskte ayrı programdır → fork + execve ile çalıştırılır.
wait: Çocuk bitmeden shell’in yeni komut almasını istemediğimizde kullanılır.
*/

#include "../minishell.h"

int	is_executable(t_cmddat *cmd_data) // gönderilen komut bulunduğu dizinde çalıştırılabilir mi bunu kontrol eden fonksiyon
{
	int	builtin; // gönderilen komut builtin mi değil mi builtin se hangi builtin komut bunun sayısal değrini tutan değişken

	builtin = get_builtin_nbr(cmd_data); // çalıştırılan komutun builtin değerini tutuyor
	if (cmd_data->file_open > 0) // eğer redirect dosyalarını açarken hata olmuşsa (file_open > 0 ayarlanıyordu)
	{
		g_exitstatus = cmd_data->file_open; // zaten komut çalıştırılamaz, global exit status bu hata koduna setlenir
		return (0); // 0 döndür fonksiyondan çık
	}
	if (!builtin && !cmd_data->full_path) // komut builtin değilse ve full_path yoksa
	{
		if (cmd_data->full_cmd[0]) // eğer komut olmadığı için komut path i bulunamadıysa
			print_err_msg_lng(cmd_data->full_cmd[0], "command not found", NULL); // gerekli hata mesajı yazılır
		g_exitstatus = 127; // exit kodu 127 olarak ayarlanır (shell de komut bulunamadı anlamı var)
		return (0); // 0 döndür ve fonksiyondan çık
	}
	else if (!builtin && cmd_data->full_path && (!access(cmd_data->full_path,
				F_OK) && access(cmd_data->full_path, X_OK) == -1)) // eğer komut bulunmuş (full_path var), dosya var (F_OK true), ama çalıştırma izni yok (X_OK=-1)
	{
		if (cmd_data->full_cmd[0]) // eğer komut varsa ve ona rağmen çalıştırılamıyorsa yani izin yoksa
			print_err_msg_lng(cmd_data->full_cmd[0], "permission denied", NULL); // gerekli hata mesajı yazılır (izin yok)
		g_exitstatus = 126; // exit code 126 olarak ayarlanır yani izin yok
		return (0); // 0 yani false döndür ve çık çünkü çalıştırılabilir değil
	}
	return (1); // eğer komutun çalıştırılmasına engel olacak hiçbir hata bulunamadıysa 1: true döndür
}

/*	Wait for first child process to terminate;
	if it exits normally (not by a signal),
	update temp_exitcode with the exit status of the child process.
	Wait for all child processes to terminate.
	If exitstatus changed while waiting, update existatus.
	F_OK checks that file exists, X_OK checks that file executable.	*/
void	wait_update_exitstatus(t_prompt *prompt) // bütün child processlerin beklendiği ve biten child processlere göre exit code ların atandığı fonksiyon
{
	int			tmp_exitstatus; // exit code u geçici olarak tutan değişken
	int			child_status; // bekleyeceğimiz child processlerin durumunu tutacak değişken
	t_cmddat	*last; // çalıştırılan son komutun içeriğini tutacak değişken (örn. pipe varsa en sağdaki komut gibi)

	tmp_exitstatus = -1; // başlangıçta -1 (geçerli değil) değeri alır
	last = cstm_lstlast(prompt->cmd_list)->data; // son komuta ilerlenip içeriği kopyalanır
	if (waitpid(prompt->pid, &child_status, 0) != -1 && WIFEXITED(child_status)) // ilk fork() edilen child process i bekliyor (!=-1) ve sinyalle değil exit ile bittiyse
		tmp_exitstatus = WEXITSTATUS(child_status); // child process in exit kodu
	while (waitpid(-1, &child_status, 0) != -1) // tüm child ları sırayla bekliyor bitene kadra hepsi
		continue ; // sadece bekliyor, hiçbir işlem yapmıyor, kaydetmiyor
	if (tmp_exitstatus != -1) // eğer ilk process normal exit ile çıktıysa
		g_exitstatus = tmp_exitstatus; // o exit code çıkış exit code u oluyor
	if (last->file_open != 0) // eğer dosya açma hatası olduysa
		g_exitstatus = last->file_open; // bu dosyanın hatasının kodunu exit code yap
	else if (!last->full_path && !get_builtin_nbr(last)) // eğer son komut hem builtin değilse hem de path i yoksa
		g_exitstatus = 127; // exit code 127: command not found olarak ayarlanır
	else if (last->full_path && !get_builtin_nbr(last)
		&& (!access(last->full_path, F_OK) && access(last->full_path,
				X_OK < 0))) // eğer path bulundu ama izin yoksa
		g_exitstatus = 126; // exit code: 127 permission denied olarak ayarlanır
	return ; // fonksiyon sonu çıkılır
}

void	cls_fds(void *content) // gönderilen komut içeriğindeki bilgilerden açık olan dosya olup olmadığını kontrol eden ve eğer açık dosya varsa kaptan fonksiyon
{
	t_cmddat	*cmd_data; // komut içeriğini geçici tutan değişken

	cmd_data = (t_cmddat *)content; // type cast ile gönderilen içeriği geçici değişkene aktarır
	if (cmd_data->infile != -1 && cmd_data->infile != 0) // eğer std inputun atandığı dosyanın fd sini tutan değişken -1: dosyayı açarken hata ya da 0: std input değilse
		close(cmd_data->infile); // verilen fd değerindeki dosyayı kapat
	if (cmd_data->outfile != -1 && cmd_data->outfile != 1) // eğer std outputun atandığı dosyanın fd sini tutan değişken -1: dosyayı açarken hata ya da 0: std output değilse
		close(cmd_data->outfile); // verilen fd değerindeki dosyayı kapat
}

/* exitstatus is 2 when misuse of shell builtins -> e.g.,
	ls: cannot access command-line argument */
void	run_cmd(void *content) // builtin olmayan ya da birden fazla iç içe komut içeren komut dizilerinin çalışmasını sağlayan fonksiyonlara yönlendiren fonksiyon
{
	t_cmddat	*cmd_data; // gönderilen komut içeriği datasının üzerinde işlemler yapabilmek için kullanılacak geçici değişken

	cmd_data = (t_cmddat *)content; // type casting yaparak kopyalıyoruz içriğini, içinde şu bilgiler var; komut ismi (full_cmd), executable path (full_path), input/output fds (infile, outfile), ve prompt (ortak shell state’i)
	if (cmd_data->prompt->stop == 1) // eğer process i durdurmak için flag daha önceki aşamalarda bir hata ya da CTRL+C sebebiyle vs true yapıldıysa
		return ; // fonksiyondan hiçbir işlem yapmadan çık
	if (!is_executable(cmd_data)) // Bu komut çalıştırılabilir mi komutta herhangi bir yazım ya da mantık hatası var mı
		return ; // hiçbir işlem başlatmadan fonksiyondan çık
	cmd_data->prompt->pid = fork(); // şuanki programı etkilemeyecek yeni bir program çalıştırılır ve ana process olmadığı için değeri pid=0 döner diğer işlemlerde de prompt içindeki bu pid değişkeninden main shell (parent process) de miyiz yoksa child process de miyiz bunun tespiti yapılır
	if (cmd_data->prompt->pid == -1) // fork() -1 döndürdüyse yani başarısız olduysa
		return ; // işlemlere devam etme fonksiyondan çık
	if (cmd_data->prompt->pid == 0) // Başarılıysa, pid=0: child process de isek yani (pid>0: parent process burada else bloğu yok çünkü parent sadece bekleyecek child processi kontrol edecek muhtemelen)
	{
		if (get_builtin_nbr(cmd_data)) // eğer child process bir builtin komutu ise
			execute_builtin(cmd_data, get_builtin_nbr(cmd_data), 1); // builtin foksiyonlarini analiz edip çalıştıran fonksiyonu forked=1 (child process ile çalıştırıldığını belirtir) değeriyle çalıştır
		dup2(cmd_data->infile, 0); // 0: std inputun nereden (cmd_data->infile içine daha önce bir fd değeri atılmışsa o yoksa std değeri 0) alınacağını ayarlayan fonksiyon çalıştırılır
		dup2(cmd_data->outfile, 1); // 1: std outputun nereden (cmd_data->outfile içine daha önce bir fd değeri atılmışsa o yoksa std değeri 1) alınacağını ayarlayan fonksiyon çalıştırılır
		cstm_lstiter(cmd_data->prompt->cmd_list, cls_fds); // açık olan fd ler kapatılıyor
		execve(cmd_data->full_path, cmd_data->full_cmd, cmd_data->prompt->envp); // external programı çağırılır, eğer başarılı olursa, bu satırdan sonrası çalışmaz child processin memorysi tamamen yeni programa yüklenir ve oradan devam edilir
		close(0); // execve çalışmazsa örn. dosya yok, izin yok vs o zaman buralar çalışır 0: stdin kapatılır
		close(1); // 1: stdout kapatılır
		exit_ms(2, cmd_data->prompt); // process g_exitstatus=2 hata durumu kodu ile biter
	}
	return ; // fonksiyondan çık
}

/*
Kullanıcının yazdığı komutları (prompt->cmd_list) alıp çalıştırmak.
İçinde hem built-in komutları (ör. cd, echo, exit …) hem de dış komutları (ls, grep vs.) yönetiyor
*/
int	execute_cmds(t_prompt *prompt) // kod çalıştırma işlemi başlamadan önce bazı analizler sonucu ön hazırlıkları da yapılmış komutların işlerinin başlaması için ana fonksiyonlara yönlendiren fonksiyon
{
	t_cmddat	*cmd_data; // prompt un içeriğini geçici olarak tutacak değişken

	if (!prompt->cmd_list || !prompt->cmd_list->data->full_cmd[0]) // eğer komut listesi boşsa ya da komut listesinin içeriğinideki komutun ilk komut stringi boşsa
		return (0); // 0 döndür ve çık
	cmd_data = prompt->cmd_list->data; // promptun içindeki komut listesindeki içeriği geçici içerik tutacak değişkene aktar
	if (cstm_lstsize(prompt->cmd_list) == 1 && get_builtin_nbr(cmd_data)) // eğer komut listesinde sadece 1 komut zinciri varsa yani tek bir işlemle yapılabilecek bir komutsa pipe redirect olmayan vb ve bu komutta builtin komutu ise (get_builtin_nbr(cmd_data) → built-in ID döndürür) tek builtin varsa fork exec yapılmaz direkt olarak parent shell içinde çalışır (örneğin cd’nin child process’te çalışmasının bir anlamı yok, çünkü parent shell’in dizinini değiştirmezdi)
	{
		g_exitstatus = execute_builtin(cmd_data, get_builtin_nbr(cmd_data), 0); // execute_builtin → komutu builtin üzerinden direkt çalıştırır get_builtin_nbr fonksiyonunun döndürdüğü değer ile hangi builtin komutunun girildiğini belirler ve o komutu çalıştıracak fonksiyona yönlendirir, sonucunu g_exitstatus değişkenine yazar
		cstm_lstiter(prompt->cmd_list, cls_fds); //  cls_fds ile komutun açık dosyaları (fd’ler) kapatılır
	}
	else // eğer 1 den fazla bağlı komut varsa veya komut builtin değilse
	{
		cstm_lstiter(prompt->cmd_list, run_cmd); // run_cmd fonksiyonu ile her komut fork+exec yapılır (veya gerekli şekilde çalıştırılır)
		cstm_lstiter(prompt->cmd_list, cls_fds); // cls_fds fonksiyonu ile pipe ve dosya fd’leri kapatılır
		wait_update_exitstatus(prompt); // child process’lerin bitmesi beklenir, exit status güncellenir
	}
	return (0); // 0 döndür fonksiyonu bitir
}
