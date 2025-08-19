/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 15:27:34 by bcili             #+#    #+#             */
/*   Updated: 2025/07/16 15:27:34 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		g_exitstatus = 0; // program boyunca çağırıldığı her file da kullanılacak olan global değişkenin tanımı yapılıyor

void	launch_minishell(t_prompt *prompt) // shell programımızın ana çalışma döngüsünü  içeren fonksiyon
{
	while (1) // her şartta döngü başlatılır ve exit fonksiyonuna girilmediği sürece kullanıcıdan komut almaya devam eder
	{
		signals_interactive(); // sinyalleri handle ettiğimiz fonksiyon, CTRL+C, D gibi
		prompt->stop = 0; // döngüleri durdurduğumuz flagi sıfırlıyoruz örneğin bir önceki komut satırından çıkılırken değeri 1 yapılmıştı
		lexer(prompt); // girilen bütün komutların parçalanmasını (tokenize) sağlayan ve hatta sistemdeki değişkenler çağrıldıysa yerlerine yerleştiren fonksiyon çağırılır
		if (prompt->stop == 0) // eğer döngüyü durdurma flagi hala 0 ise yani lexer komutları bölme işleminde bir sorun çıkmadıysa
			parser(prompt, 0, 0); // bölünmüş haldeki komutlar yani tokenler analiz edilir pipe ve redirectler handle edilir
        if (cstm_lstsize(prompt->cmd_list) > 1 && prompt->stop == 0) // eğer birden fazla birbirine bağlı ama kendi başlarına da farklı anlamlar taşıyan komut varsa (| kullanılarak birleştirilmiş gibi)
			pipe_infile_outfile(prompt->cmd_list); // kullanılan pipe için okuma ve yazma uçlarını analiz edip değerlerini ayarlar
		if (!prompt->stop) // eğer döngü durdurma flagi bu işlemlerden sonra da hala 0 ise yani girilen komutlarda mantık hataları da yok ise
        	execute_cmds(prompt); // artık komutları çalıştırma işlemlerinin yapıldığı fonksiyon çalıştırılır
		prompt->cmd_list = NULL; // yeni satıra geçilmeden önce son olarak önceki satırdaki komut listemiz temizlenir
	}
}

/* no garbage collection for below function as it does not leak
set outfile to write end of pipe
set infile to read end of pipe */
void	pipe_infile_outfile(t_node *cmd_lst) // komut listesindeki her komutu bir sonraki ile | (pipe) üzerinden bağlamak
{
    int	*pip; // pipe() fonksiyonunun döndürdüğü okuma 0 ve yazma 1 uçlarının değerini tutacak integer array
    
	while (cmd_lst->next != NULL) // komut listesinin sonuna gelene kadar gez
	{
        pip = ft_calloc(2, sizeof(int)); // 2 tane integer kadar pip değişkenine yer ayır
		if (!pip) // eğer yer ayrılırken hata olduysa
        	return ; // fonksiyondan çık
		if (pipe(pip) == -1) // pipe fonksiyonu başarısız olursa
		{
            free(pip); // ayrılan belleği serbest bırak
			return ; // fonksiyondan çık
		}
		if (cmd_lst->data->outfile == 1) // eğer pipe ın yazma ucu daha önce ayarlanmamışsa
	        cmd_lst->data->outfile = pip[1]; // fonksiyondan dönen yazma ucu değeri atanır
		else // eğer daha önce atandıysa
    	    close(pip[1]); // yazma işi bitmiş olan dosyayı kapat
		if (cmd_lst->next->data->infile == 0) // eğer pipe ın giriş değeri daha önce ayarlanmadıysa
        	cmd_lst->next->data->infile = pip[0]; //  okuma ucu için dönen değer atılır (okunacak dosyanın fd değeri)
		else // eğer okuma ucu daha önce ayarlandıysa
        	close(pip[0]); // okuma işlemi bitmiş demektir dosya kapatılır
		cmd_lst = cmd_lst->next; // somraki komuta ilerle
		free(pip); // pip değişkenine ayrılan alanı srbest bırak
	}
	return ; // fonksiyon bitti çık
}

int	main(int argc, char *argv[], char **envp) // env = environment, ortam değişkenlerini içeren bir string dizisi (HOME/bcili/..., PATH, etc.)
{
    t_prompt	*prompt; // shell in çalışması için gerekli tüm bilgileri içeren değişken declare ediliyor

    prompt = NULL; // allocation yapılmadan önce başlangıç değeri veriliyor
    (void)argv; // argüman kullanılmayacağı için unused variable uyarısını engeller
    if (argc == 1) // sadece ./minishell komutu varsa çalışır
    {
        prompt = ft_calloc(1, sizeof(t_prompt)); // prompt için dinamik bellek (heap)'te t_prompt büyüklüğünde yer ayrılıyor
        init_prompt_struct(prompt, envp); // yer ayrılan prompt içerisindeki değişkenler için shell döngüleri başlamadan önce başlangıç değerleri atanıyor
    }
    else // ./minishell komutu dışında herhangi bir başka argüman varsa kabul edilmiyor programdan çıkılııyor
    {
        printf("Error! This Program Does Not Accept Any Argument.\n"); // fazladan argüman için uyarı mesajı yazdırılıyor
        exit(0); // error koduyla programdan çıkılıyor
    }
    launch_minishell(prompt); // shell için gerekli döngülerin başlatıldığı ve bütün komutların handle edildiği asıl isi yapam fonksiyon çağırılıyor
    return (0); // program başarılı biterse ana fonksiyon 0 döndürüyor
}
