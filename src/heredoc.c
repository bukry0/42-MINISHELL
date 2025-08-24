/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:15:58 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:15:58 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*

Heredoc Nedir?
Heredoc (Here Document), shell’de bir komuta standart input (stdin) sağlamak için kullanılan bir özel yönlendirme türüdür.
Normalde input’u ya klavyeden ya da bir dosyadan alırız, fakat heredoc sayesinde input’u kodun içine gömebiliriz. Örneğin;

cat << EOF
Merhaba dünya
Burası heredoc örneği
EOF

<< EOF → delimiter (sınırlandırıcı).
Kullanıcı EOF yazana kadar girilen her satır cat komutuna stdin olarak aktarılır.

Çıktı:
Merhaba dünya
Burası heredoc örneği

Avantajı: Script veya shell içinde çok satırlı input sağlamak kolaylaşır.
Ayrıca heredoc içinde environment variable’lar ($USER gibi) expand edilebilir (bash’de bu varsayılan davranıştır).

Senin Kodunun Genel Akışı
Senin minishell’de heredoc şu şekilde işliyor:
Kullanıcı << LIMITER yazarsa → heredoc başlatılır.
launch_heredoc → heredoc sürecini başlatır (limiter kontrolü).
get_heredoc → kullanıcıdan satır satır input alır (readline("> ") ile).
Eğer satır limiter’a eşitse heredoc biter.
Değilse, environment variable expand edilir ve stringe eklenir.
pipe_heredoc → heredoc içeriğini bir pipe içine yazar ve okuma ucunu (fd) döner.
Böylece heredoc, sanki bir dosyadan okunuyormuş gibi çalışır.

zaten shell’in en başında signal(SIGINT, handler) gibi ayarlamalar yapmışsın, evet.
Ama heredoc sırasında özel bir durum var:
Normal shell prompt açıkken → CTRL+C yaparsan tüm komutu iptal edip yeni prompt’a dönmesi gerekiyor.
Heredoc sırasında → CTRL+C yaparsan heredoc girişini iptal edip shell’e geri dönmesi gerekiyor (ama shell’i komple kapatmaması lazım).

CTRL+\ (quit) heredoc sırasında genelde görmezden gelinir.
CTRL+D (EOF) → heredoc’u bitirir.
İşte bu yüzden heredoc girerken signal davranışını geçici olarak değiştiriyoruz:

signals_interactive() → heredoc için uygun signal ayarlarını açar. (örn. CTRL+C → heredoc’u kes, ama shell’i kapatma)
signals_non_interactive() → heredoc bitince signal’leri normale döndürür (senin en başta kurduğun default shell signal ayarları).
Yani heredoc sırasında sinyaller farklı yorumlanmalı. Eğer bu ayrımı yapmazsan:
CTRL+C bastığında ya tüm minishell kapanır, ya da heredoc içinden çıkmaz → ikisi de hatalı olur.

Ayrıca heredoc içerisinde de env değişkenlerinin içeriklerine erişilebilmesi mümkündür
O sebeple expand_var ile eğer env değişkenlerinin değerleri kullanılmak istenirse değerleri metin içine yerleştirilir

*/

void	launch_heredoc(t_prompt *prompt, t_cmddat *cmd, int i) // kullanıcıdan << redirecti alındığında girilen limiter doğru mu diye kontrol edip ona göre input bekleyicisi çalıştıracak fonksiyon
{
	char	*lim; // here doc için input sonuna gelindiği anlaşılacak stringi tutar
	size_t	j; // limiter içinde gezerek doğru formatta mı yazıldığını kontrol ederken kullanılacak index

	lim = cmd->full_cmd[i + 1]; // i. index redirect karakterini tuttuğu için bir sonraki argüman limiter olacaktır onu lim içine atarız örn; arg[0]: cat, arg[1]: << arg[2]: END
	j = 0; // ilk indexten başla
	while (ft_isalnum(lim[j])) // bütün karakterler alfabetik ya da nümerikse ilerle
		j++; // sonraki karaktere geç
	if (j != ft_strlen(lim)) // eğer bütün karakterler alfabetik ya da nümerik değilse limiter formatına uygun bir limiter girilmemiş demektir örneği cat << -, - karakteri limiter olamaz
	{
		ft_putstr_fd("minishell: input error: delimiter must ", 2); // hata mesajı 2: error ucuna yazılır
		ft_putstr_fd("contain only alphanumeric characters\n", 2); // hata mesajı 2: error ucuna yazılır
		prompt->stop = 1; // stop flagi true yapılır ki bu fonksiyondan sonra işlem devam etmesin yeni satıra geçilsin
		return ; // fonksiyondan çık
	}
	cmd->infile = get_heredoc(prompt, lim); // heredoc başlat yani limiter görene kadar kullanıcıdan input alacak fonksiyonu çağır
	if (g_exitstatus == 1) // eğer heredoc esnasında ctrl+c gibi bir sinyal yakalanırsa
		prompt->stop = 1; // stop flagi true yapılır
}

int	get_heredoc(t_prompt *prompt, char *lim) // limiter stringi ile karşılaşana kadar kullanıcıdan input almaya devam edecek fonksiyon
{
	char	*content; // limiter stringi girilmeden öncesinde input olarak girilen içeriği tutacak değişken
	char	*line; // satır satır inputu alan değişken sonrasında içeriği content içinde birleşecek

	content = NULL; // başlangıç değeri NULL
	line = NULL; // başlangıç değeri NULL
	g_exitstatus = 0; // exit code başlangıçta 0 başarılı olarak ayarlanır
	while (1) // sinyall çıkışı ya da limiter stringi girilmediği sürece açık olarak bekler
	{
		signals_interactive(); // heredoc içerisinde aktif input alımı yapılırken sinyal girilirse handle et (CTRL+C vb)
		line = readline("> "); // kullanıcıdan satır almak için bekle
		collect_grbg(prompt, line); // girilen satır için readline tarafından ayrılan hafızayı program sonunda freelemek için garbage listesi içerisine al
		signals_non_interactive(); // kullanıcıdan satır alındı artık özel sinyal davranışını geri al
		if (!line) // eğer satırda kullanıcı CTRL+D yaparsa
		{
			print_err_msg("warning", "here-document delimited by end-of-file"); // limiter stringinden önce başka bir çıkış sinyali alındığını belli eden hata mesajı yazılır
			break ; // input alınmaya devam etmesi için döngüden çıkılır şu ana kadarki alınan inputla işlem devam eder limiter ile değil sinyalle sonlanmış olur input alımı
		}
		if (!ft_strncmp(line, lim, ft_strlen(line))
			&& ft_strlen(line) == ft_strlen(lim)) // eğer girilen satır ile limiter içeriği eşleşiyorsa yani limiter stringi girildiyse
			break ; // döngüden çık çünkü sonlandırmak için belirlediği string girildi 
		line = expand_var(prompt, line, prompt->envp, 0); // eğer girilen satırda env değişkenlerinden herhangibirinin değeri çağırıldıysa tıpkı normal satırlardaki gibi $USER gibi mesela onların env içindeki karşılıklarıyla stringi güncelleyip döndürür
		content = add_to_str(prompt, &content, line); // content devamına inputtan okunan satır eklenir
		content = add_to_str(prompt, &content, "\n"); // sonuna da nexline koyulur
	}
	return (pipe_heredoc(content)); // heredoc mantığıyla input olarak alınan satırları sanki bir dosyadan okunmuş gibi gösterecek (yani tıpkı pipe gibi input ucu terminal değilde sanki bir dosyaymış gibi) olan fonksiyon çağırılır ve onun döndüreceği fd değeri döndürülür
}

int	pipe_heredoc(char *content) // gönderilen içerik << redirect ile terminalden alındı fakat işlevinde sanki dosyadan okunmuş gibi görünmesi için ona bir fd değeri atayıp sanki dosyaymış gibi davranmasını sağlayacak fonksiyon
{
	int	pip[2]; // pip[0]: pipe ın okuma ucu pip[1]: pipeın yazma ucu olarak kullanılacak pipe() fonksiyonu için

	if (g_exitstatus) // eğer sinyal tarafından heredoc kesilmişse
		return (0); // 0 döndür ve çık
	if (!pipe(pip)) // pip içindeki değerlerle okuma ve yazma ucu oluşturan bir pipe aç yani child process devam ediyor artık
	{
		ft_putstr_fd(content, pip[1]); // pip[1] içinde seçilen yazma ucuna content içeriğini yaz
		close(pip[1]); // içeriği kopyaladığın dosyayı kapat
		return (pip[0]); // pipe ın okuma ucunu döndür infile fd olacak
	}
	return (0); // eğer pipe başarılı değilse 0 döndür çık
}
