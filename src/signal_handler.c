<<</* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:25:43 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:25:43 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
Sinyaller Nedir?
Sinyal, işletim sisteminin bir programa gönderdiği küçük bir uyarı ya da kesme sinyali gibidir.
Amacı: “Bir şey oldu, haberin olsun!” demek.
Bu şey bazen klavyeden olur (CTRL+C gibi), bazen programdan olur (bir hata gibi), bazen başka bir işlem gönderir.İşlem bu sinyali aldığında ya;
Varsayılan davranışı uygular (örneğin kendini sonlandırır),
Veya sen o sinyale özel bir handler (yakalama işlevi) tanımladıysan, o işlevi çalıştırır.

Hangi Durumlarda Sinyal Gönderilir?
İşletim sistemi bazı olaylarda sinyal gönderir:

Olay										Sinyal				Açıklama
Kullanıcı Ctrl+C tuşuna basar				SIGINT				Kesme sinyali – genelde işlemi sonlandırır.
Kullanıcı Ctrl+\ tuşuna basar				SIGQUIT				İşlemi sonlandırır ve core dump üretir.
Bir işlem kill komutu ile sonlandırılır		SIGTERM, SIGKILL	Dışarıdan sinyal gönderilmiştir.
Hatalı bellek erişimi						SIGSEGV				Segmentation fault oluşmuştur.
Çocuk işlem biter							SIGCHLD				Ebeveyn işlemi haberdar eder.

Neden Handle Etmeliyiz?
Kendi terminal uygulamanı yazarken sinyalleri doğru kontrol etmezsen;
Ctrl+C ile shell’in çökmesine neden olabilirsin.
Arka planda çalışan child process’leri düzgün öldüremezsin.
Prompt'un yapısı bozulabilir ($ satırı karışabilir).
Örneğin:

bash kodu;
$ ./minishell
$ ls -la
^C     <- bu sırada işlem düzgün sonlandırılmazsa prompt bozulur

Sinyal Yakalama (Signal Handling)
Bir sinyali yakalamak için signal() ya da sigaction() fonksiyonunu kullanırsın, örneğin;

void	handle_sigint(int sig) // Mınıshell projemizle alakası yok sadece örnek bir kod
{
	printf("Ctrl+C yakalandı! Sinyal: %d\n", sig);
}

int main()
{
	signal(SIGINT, handle_sigint); // bu fonksiyon bir kez çağırıldıktan sonra başka hiç bir işleme ihtiyacımız yok
								   // artık bu fonksiyon program boyunca gelecek sinyalleri yakalamaya hazır bir halde beklemede
								   // yukarıda açıklaması olan SIGINT tipindeki değişkenleri yakalayıp handle_sigint fonksiyonuna aktaracak
								   // bu fonksiyonda bazı sinyallere özel işlevler atanmış olabilir eğer öyleyse atanmış olan işlem gerçekleştirilir
								   // eğer özel bir işlem atanmadıysa da bu sinyalin varsayılan işlemi varsa o gerçekleştirilir
								   // eğer klavyeden bir tuşa basılırsa bu fonksiyon onu hemen yakalayacak ve kontrol edecek
								   // eğer girilen tuşun bir görevi varsa o görev direkt gerçekleştirilmeye başlanacak
	while (1) // sonrasında bu while içine dögüye girilir ama signal olan satırı geçmiş olsak bile hala o fonksiyon da arka planda çalışmaya devam eder
	{
		printf("Çalışıyorum...\n"); // gözlemlemek adına döngü boyunca metni tekrar tekrar yazdırırız
		sleep(1); // 1 sn (saniye olduğundan emin değilim hahasjh) beklenir döögü tekrar başlatılmadan önce
	}
}

Çıktı:
bash kodları;

Çalışıyorum...
Çalışıyorum...
^C
Ctrl+C yakalandı! Sinyal: 2
Çalışıyorum...
*/


void	signals_interactive(void) // komut satırında henüz komut girilmemişken CTRL+C ye basıldığında shell çökmesin diye kendi handle ettiğimiz fonksiyon
{
	signal(SIGINT, handle_sig_int); // SIGINT sinyali yakalandığında shell in varsayılan işi ile shell in çökme ihtimali var ve biz bunu ortadan kaldırmak için handle_sig_int fonksiyonuna yönlendiriyoruz bu sinyali yakaladığımızda
	signal(SIGQUIT, SIG_IGN); // SIGQUIT sinyali gönderildiğinde (CTRL+\) zaten yeni ve boş bir komut satırında olduğumuz için SIG_IGN ignore yani yok sayarız bu sinyalleri
}

void	signals_non_interactive(void) // çalışan bir komut varken yani aktif bir fork() ve execve() varken girilen sinyallerin handle edilmesini sağlayan fonksiyon
{
	signal(SIGINT, handle_sig_quit); // komutlar çalışırken girilen SIGINT sinyali için handle_sig_quit fonksiyonu ile hemen çalışan döngüleri durdurup yeni bir komut satırı açıyoruz
	signal(SIGQUIT, handle_sig_quit); // aynı şekilde SIGQUIT sinyali girildiğinde de handle_sig_quit fonksiyonuyla çalışan döngüleri direkt kapatıp core dump hata mesajı yazılır sonra da yeni komut satırı açılır
}

/*	Ctrl+\
	131	*/
void	handle_sig_quit(int n) // bir komutun çalışması esnasında yarıda kapatmak için sinyal gönderilirse döngüleri durdurup yeni satır başlatacak fonksiyon
{
	if (n == SIGQUIT) // gönderilen sinyal SIGQUIT sinyali ise 
		printf("Quit (core dumped)"); // programın core dump ile sonlandırılması gerektiği için ekrana mesaj olarak yazarız
	write(1, "\n", STDERR_FILENO); // yeni satıra geçer
	rl_on_new_line(); // yeni komut satırı için gerekli değişkenleri sıfırlar ve yeni komut satırını başlatır
}

/*	Ctrl+c, interrupt	*/
void	handle_sig_int(int n) // zaten yeni bir komut satırındayken yani fork() exec() gibi işlemler aktif değilken gönderilen CTRL+C sinyalini ele alan fonksiyon
{
	if (n == SIGINT) // eper gönderilen sinyal CTRL+C ise
	{
		rl_replace_line("", 0); // readline satırını başlangıç haline getir, değişkenlerin içini temizle
		rl_on_new_line(); // yeni komut satırına geç
		write(1, "\n", STDERR_FILENO); // new line yaz, STDERR_FILENO: 2 numaralı standart error çıkışı yani ekrana da yansıtılan hata mesajları
		rl_redisplay(); // promptu yeniden başlat yani ekrana yeni komut satırı promptunu yazdır ve komut beklemeye başla
	}
}
