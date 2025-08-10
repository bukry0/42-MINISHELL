/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_minishell.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:08:25 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:08:25 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


/*
$? nedir, $? son çalıştırılan foreground komutun çıkış kodudur (exit status). Shell bunu kullanıcıya echo $? ile gösterir.
Değer tipik olarak 0 = başarı, >0 = hata (örneğin 1, 2, …). İşletim sistemi içindeki uygulama exit(n) ile bu değeri belirler.

Exit code nereden gelir? (temel kaynaklar)
Program normal sonlanırsa: program exit(n) çağırmış veya return n; döndürmüşse n değerini alırsın. (C’de main'den return 5; -> exit code 5.)
Program sinyal ile öldürülürse: örn. SIGINT ile öldü → shell bunun yerine 128 + signal_number koyar (bu yaygın konvansiyon; bash de böyle yapar).
Örneğin;
SIGINT = 2 → exit status = 130.
SIGQUIT = 3 → exit status = 131.

exec/başarısızlık durumları: komut bulunamazsa shell genelde 127 döndürür; komut var ama çalıştırılamıyorsa 126.
Exit kodu bir byte (0–255) ile tutulur. Büyük sayılar 256’ya göre modulo alınır.

waitpid() ile nasıl alırsın — (en kritik kısım)
fork() sonrası execve() ile bir child başlatırsın. Ana (parent) waitpid/wait ile child’in sonlanmasını bekler ve OS’nin döndürdüğü status kodunu yorumlarsın:

	int status;
	pid_t pid = waitpid(child_pid, &status, 0);

	if (pid == -1)
		perror("waitpid"); // hata
	if (WIFEXITED(status))
	{
	    // Normal çıkış
    	g_exitstatus = WEXITSTATUS(status); // 0..255
	}
	else if (WIFSIGNALED(status))
	{
    	// Sinyal ile öldü
    	int sig = WTERMSIG(status);
    	g_exitstatus = 128 + sig; // yaygın konvansiyon (bash ile uyumlu)
    	// optionally: if (WCOREDUMP(status)) -> core dump bilgisi
	}

WIFEXITED(status) ve WEXITSTATUS(status) makroları çıkış (exit(n)) durumunu verir.
WIFSIGNALED(status) ve WTERMSIG(status) makroları sinyal sebebiyle sonlanmayı verir.

Pratik örnekler — ne beklemelisin
Komut normal sona erdi, bash;
$ /bin/true
$ echo $?
0

Komut hata kodu dönüyor, bash;
$ /bin/false
$ echo $?
1

Komut bulunmuyor, bash;
$ nosuchcmd
$ echo $?
127   # tipik

Komut sinyal ile öldürülür, bash;
$ sleep 10
# Ctrl+C bastın -> SIGINT (2)
$ echo $?
130   # 128 + 2

SIGQUIT (Ctrl+) ile, bash;
# bir child core dump ile biterse:
$ echo $?
131   # 128 + 3

Pipeline örneği (bash davranışı);
$ false | true
$ echo $?   # pipeline'ın son komutunun exit status'u döner
0

(Genel minishell için g_exitstatus'ı pipeline’daki son komutun sonucu yapmalısın.)

Built-in’ler (cd, echo, export...) ve $?
Built-in’ler shell process içinde çalışır (yani child fork’lanmadan çalıştırılırlarsa).
Bu yüzden built-in fonksiyonları çağırdığında onların dönüş değerlerini (return ya da fonksiyon sonucu) doğrudan g_exitstatus olarak ayarlamalısın:

	int cstm_cd(...)
	{
	    if (chdir(path) == -1)
		{
	        perror("cd");
    	    g_exitstatus = 1;
        	return 1;
    	}
    	g_exitstatus = 0;
    	return 0;
}

Eğer built-in’leri fork’layıp child’da çalıştırıyorsan, child’in exit status’u waitpid ile gelmelidir (yukarıda anlatıldığı gibi).

Pipeline ve birden fazla child beklemek
Eğer pipe ile birden fazla child oluşturduysan, genelde tüm child’ları wait edip sonra g_exitstatus'ı son foreground komutun exit status'u ile ayarlarsın. Basit yol:

	int status;
	pid_t wpid;
	int last_status = 0;
	while ((wpid = waitpid(-1, &status, 0)) > 0)
	{
	    if (WIFEXITED(status))
    	    last_status = WEXITSTATUS(status);
    	else if (WIFSIGNALED(status))
	   	    last_status = 128 + WTERMSIG(status);
	}
	g_exitstatus = last_status;

Bu tüm çocukları bekler ve en son görülen çocuğun durumunu g_exitstatus yapar. (Bash daha karmaşık ama bu yeterli.)

CTRL+C (SIGINT) iki durumda farklı davranır — g_exitstatus nasıl etkilenir?
A) Eğer child komut çalışıyorsa ve kullanıcı Ctrl+C basıyorsa child SIGINT alır ve ölür → waitpid ile shell %? = 130 şeklinde alır (WIFSIGNALED -> 128 + sig).
B) Eğer prompt sırasında (yani hiçbir child çalışmıyor) kullanıcı Ctrl+C basarsa bash genelde g_exitstatus = 130 olarak ayarlar. Bunu yapmak için:
Signal handler SIGINT'i yakalar ve bir global g_signal ayarlar,

Main döngüsünde handler gördüğünde g_exitstatus = 130; atar ve prompt'u yeniden gösterir.
Böylece echo $? Ctrl+C sonrasında 130 döner.

Not: subject'te sinyal handlerlarda global değişken sınırlaması vardı; bu yüzden g_signal saklanıp main loop'ta g_exitstatus düzenlenmelidir.

exit() ile sonlanma, byte sınırı
exit(257) kullanılırsa OS exit code = 257 & 0xFF = 1 olur. Yani exit kodu 0–255 aralığında değerlendirilir.
Bu yüzden WEXITSTATUS(status) 0..255 değer verir.

Özet – Mini-shell'de yapman gerekenler (pratik adımlar)
Child çalıştırma (execve): fork() -> execve() -> parent waitpid(child, &status, 0) -> status yorumla (WIFEXITED/WIFSIGNALED) -> g_exitstatus = WEXITSTATUS(status) veya 128 + WTERMSIG(status).
Pipeline: fork’ladığın tüm çocukları bekle; g_exitstatus'ı son foreground komutun sonucuna ayarla.
Built-in’ler (parent içinde çalışıyorsa): fonksiyon dönüşünü g_exitstatus olarak ayarla.
CTRL+C prompt durumu: signal handler bir g_signal set etsin; main loop bunu görünce prompt davranışını sağlar ve g_exitstatus = 130 atar.
Command not found / permission: exec başarısız olursa (ENOENT -> 127; EACCES -> 126 gibi) uygun g_exitstatus ataması yap.

Sık karıştırılanlar — kısa doğrulamalar
SIGINT (Ctrl+C) ile ölüm → $? = 128 + 2 = 130.
SIGQUIT (Ctrl+) ile ölüm → $? = 128 + 3 = 131.
echo $? son foreground komutun durumunu gösterir (pipeline'da son komut).
Built-in'ler parent'da çalışıyorsa onların dönüşü $? olur.

*/


/*free and exit*/
void	exit_ms(int g_exitstatus, t_prompt *prompt)
{
	free_all(prompt);
	exit(g_exitstatus);
}

void	free_all(t_prompt *prompt)
{
	free_grbg(prompt->grbg_lst);
	rl_clear_history();
	if (prompt)
		free(prompt);
}

void	free_node_list(t_node *head)
{
	t_node	*current;
	t_node	*temp;

	current = head;
	while (current)
	{
		temp = current;
		current = current->next;
		if (temp && temp->data)
		{
			if (temp->data->full_cmd)
				free(temp->data->full_cmd);
			if (temp->data->full_path)
				free(temp->data->full_path);
			free(temp->data);
		}
		free(temp);
	}
}
