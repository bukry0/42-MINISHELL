/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cstm_echo.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 15:56:05 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 15:56:05 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
Echo nedir?
echo komutu, terminale metin veya argümanları yazdırmak için kullanılan en temel shell komutudur.
Varsayılan davranışı:
Argümanları aralarında boşluk ile birleştirip yazar.
En sona newline (satır sonu) ekler. Örn;

echo Merhaba Dünya
Çıktı:
Merhaba Dünya

Genel Kullanımı
echo [seçenekler] [argümanlar]
Önemli seçenekler:
-n → Satır sonunda newline basmaz.
(Gerçek bash’te ayrıca -e ve -E seçenekleri de vardır → escape karakterlerini işleyip işlememek için. Ama senin minishell’inde bu yok.)

Senin minishell’deki echo
Senin yazdığın cstm_echo fonksiyonunda şu davranışlar var:
Argüman yoksa
echo
Çıktı:

(sadece boş bir satır)

Normal argümanlar
echo Merhaba
Çıktı:
Merhaba

echo Merhaba Dünya
Çıktı:
Merhaba Dünya

-n seçeneği
echo -n Merhaba
Çıktı:
Merhaba

(newline yok, yani imleç aynı satırda kalır)

echo -n Merhaba Dünya
Çıktı:
Merhaba Dünya%

(% burada bash prompt’un göstergesidir → newline atılmadığı için aynı satırda kalıyor)

Teknik Detaylar
Komut, cmd_data->full_cmd dizisinden argümanları okur:
full_cmd[0] = "echo"
full_cmd[1] = ilk argüman
full_cmd[2] = ikinci argüman
Eğer -n varsa:
nl = 0 yapılır, yani en sona newline eklenmez.
Argümanlar yazılırken aralarına tek boşluk konur.
Yazma işlemi normalde stdout’a yapılır, fakat redirect edilmişse → cmd_data->outfile’a yapılır. Örn;

echo Merhaba > out.txt
→ çıktı terminale değil out.txt dosyasına gider.

Çıkış Durumu (Exit Status)
Başarıyla çalıştığında 0 döner.
Hata olursa (örneğin cmd_data NULL ise) → 1 döner.

Notlar
Bash’te echo aslında çok daha karmaşıktır:
echo -nnn (birden fazla -n)
echo -e → \n, \t gibi escape’leri işler
echo -E → escape’leri iptal eder
Senin minishell’de sadece temel kullanım + -n desteği vardır.
Bu, proje gerekliliklerini karşılar çünkü genelde zorunlu olan sadece -n’dir.

Örnekler Tablosu
Komut					Çıktı										Açıklama
echo					(boş satır)									Argüman yok, sadece newline basılır
echo Merhaba			Merhaba										Tek argüman
echo Merhaba Dünya		Merhaba Dünya								Birden fazla argüman boşlukla birleşir
echo -n Merhaba			Merhaba										Newline yok
echo -n Merhaba Dünya	Merhaba Dünya (aynı satırda prompt gelir)	-n yüzünden newline basılmaz
echo Hello > out.txt	(terminalde hiçbir şey yok)					Çıktı out.txt dosyasına gider

*/

int	cstm_echo(t_cmddat *cmd_data) // echo (yazma) komutunun işlevini yapan fonksiyon
{
	int	nl; // sonuna newline koyulsun mu flagi
	int	i; // argüman indexi

	nl = 1; // başlangıç değeri true yani newline koyulsun
	i = 1; // full_cmd[0] = "echo" olduğu için 1 ile başlar
	if (cmd_data == NULL) // eğer komut içeriği boşsa
		return (1); // 1 döndür ve çık
	if (cmd_data->full_cmd[1] == NULL) // eğer yazılması istenen argüman boşsa
		return (ft_putstr_fd("\n", cmd_data->outfile), 1); // ekrana hiçbir karakter yazma sadece newline yaz ve 1 döndürerek çık
	if (!ft_strcmp(cmd_data->full_cmd[1], "-n")) // echo sonrasındaki komut -n flagi ise 
	{
		nl = 0; // sonuna newline koyulması istenmiyor demektir bu yüzden flag false a çevrilir
		i += 1; // sonraki komuta ilerle
	}
	while (cmd_data->full_cmd[i]) // eğer elimizdeki sıradaki değişken boş değilse
	{
		ft_putstr_fd(cmd_data->full_cmd[i], cmd_data->outfile); // yazılması istenilen metini eğer redirect varsa verilen fd değerinin gösterdiği dosyay yoksa da varsayılan olan std output yani ekrana yazılır
		i += 1; // sonraki komuta geç
		if (cmd_data->full_cmd[i] != NULL) // eğer sonraki yazılacak komut (aslında metin) boş değilse 
			ft_putstr_fd(" ", cmd_data->outfile); // aralarında normalde bulunan ama split ile seperator olarak kullandığımız için kaybettiğimiz space değişkenini yazdır
	}
	if (nl) // bütün metin yazıldı eğer newline flagi true ise (varsayılan da true)
		ft_putstr_fd("\n", cmd_data->outfile); // en sona newline yaz
	return (0); // 0 döndürerek fonksiyondan çık
}
