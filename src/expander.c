/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:10:19 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:10:19 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


/*
env nedir?
env kelimesi, “environment variables” (çevre değişkenleri) anlamına geliyor. Çevre değişkenleri, işletim sisteminin ve programların
birbirleriyle bilgi paylaşmasını sağlayan anahtar = değer çiftleridir. Örneğin;

PATH=/usr/local/bin:/usr/bin:/bin
HOME=/home/kullanici
USER=kullanici
SHELL=/bin/bash

Burada;
PATH: Program çalıştırma yollarını tutar. Terminalde ls yazdığında, ls komutunun hangi klasörde olduğunu bulmak için PATH kullanılır.
HOME: Kullanıcının ana dizini.
USER: Kullanıcı adı.
SHELL: Varsayılan kabuk.

Sen programını main(int argc, char **argv, char **env) şeklinde yazdığında 3. parametre olan env, işletim sisteminin sana bu değişkenleri
string array olarak veriyor. Yani env aslında şuna benziyor;
env[0] = "PATH=/usr/local/bin:/usr/bin:/bin";
env[1] = "HOME=/home/kullanici";
env[2] = "USER=kullanici";
env[3] = NULL; // Bittiğini gösterir

$ işaretleri ne işe yarıyor?
Shell dünyasında $ değişken okuma operatörüdür.
$HOME → HOME değişkeninin değerini getirir.
$USER → kullanıcı adını getirir.
$PATH → PATH değişkenini getirir.
$? → Son çalıştırılan komutun çıkış kodunu (exit status) verir.
Örneğin, bash;
echo $HOME
# çıktı: /home/kullanici

echo $USER
# çıktı: kullanici

ls asdf
# ls: cannot access 'asdf': No such file or directory
echo $?
# çıktı: 2   (hata kodu)

Shell neden $ gördüğünde değiştirme yapar?
Buna variable expansion denir (değişken genişletme).
Shell, $ ile başlayan ifadeleri ilgili değişkenin değerine çevirir. Örneğin, Bash;

echo "Merhaba $USER, evin: $HOME"
Shell bunu çalıştırmadan önce şu hale getirir:

echo "Merhaba kullanici, evin: /home/kullanici"
Sonra echo komutu bu metni ekrana basar.

Senin bu kodlarda yaptığın şey ne?
Sen kendi yazdığın minishell’de aynı mekanizmayı implement ediyorsun. Yani:

Kullanıcı bir komut yazıyor:

echo Merhaba $USER
Sen bu stringi okuyorsun.

String içinde $ var mı diye bakıyorsun.
Eğer $ varsa, $'den sonra gelen ismin uzunluğunu buluyorsun (get_len_var ile).
Bu ismi alıp, envp dizisinde karşılığını buluyorsun.
Bulduğun değeri stringin içine yerleştiriyorsun (create_sub_var ile).
Özel bir durum: $? → bu global değişken g_exitstatus ile tutuluyor ve handle_g_exitstatus ile ekleniyor.

Neden "parçalamak" zorundayız?
Çünkü $ sadece değişkenin adını temsil ediyor, değerini değil, örneğin;

"echo $USER" → senin programın önce $USER'i USER değişkeninin değeri ile değiştirmeli.
Eğer değiştirmezsen, execve fonksiyonu $USER diye bir dosya arar, bulamaz → hata olur.
Yani expander fonksiyonları, komut çalışmadan önce $'li kısımları gerçek değerleriyle değiştirip, komutun doğru çalışmasını sağlıyor.

Basit örnek üzerinden işleyiş:
Diyelim env şu şekilde:

env[0] = "USER=ahmet";
env[1] = "HOME=/home/ahmet";
env[2] = NULL;

Kullanıcı şunu yazdı:
echo $HOME/Documents

Adımlar:
Program $ işaretini gördü → bunun ardından HOME kelimesi var.
HOME'un değerini env içinde buldu → /home/ahmet.
$HOME/Documents → /home/ahmet/Documents oldu.

Son komut:
echo /home/ahmet/Documents
olarak execve'ye gidecek.

*/



/*
  Expands environment variables in the array of strings `str` using
  the environment variables `ev`.
  It creates a new array with expanded strings and returns it.

  Parameters:
    - str: Array of strings to be expanded.
    - ev:  Array of strings representing environment variables.

  Returns:
    - A new array of strings with expanded environment variables.
*/

char	**expander(t_prompt *prompt, char **str, char **ev) // str içindeki anlamlı parçalara bölünmüş kodların içinde expanded değişkenlerin ($ ile başlayan bash değişkenler) sistemdeki değerlerini yerine yazıp güncelleyen fonksiyon
{
	int		i; // komut stringi içinde gezinmek için index
	char	**temp; // komut stringinde değiştirilmesi gereken değerler varsa onların son hallerinin aktarılacağı değişken

	temp = get_grbg(prompt, get_len_arr(str) + 1, sizeof(char *)); // temp string dizisi için de daha önce oluşturulan komut dizisi kadar yer ayrılır
	if (!temp) // eğer yer ayrılırken bir sorun oluştuysa
		return (NULL); // NULL ile çık
	i = 0; // indexi sıfırla
	while (str[i]) // ilk komut parçasından başla sonuna kadar ilerle
	{
		temp[i] = expand_var(prompt, str[i], ev, 0); // gönderilen komut parçasında bir $değişken_adı tipinde ortam değişkeni tespit edilirse onun değerini sistemden alıp yerine yazarak gönderilen kod parçasının güncellenmiş halini gönderen fonksiyon
		i++; // bir sonraki komut stringine geç
	}
	temp[i] = NULL; // son komuttan sonrasına kapanış için NULL koy
	i = 0; // indexi sıfırla
	while (temp[i] != NULL) // komut parçalarının sonuna kadar ilerle tekrar
	{
		if (ft_strcmp(temp[i], "") == 0) // eğer komut parçalarından boş olan varsa örneğin sistemde karşılığı bulunmayan bir değişken gönderdik ve sonucunda NULL aldık
		{
			del_str(temp, i, 1); // bu boş komut parçasını aradan çıkart
			i = 0; // sildikten sonra komut parçası atlamamk adına başa geç
		}
		else // eğer elimdeki komut parçası boş değilse
			i++; // bir sonraki komuta geç
	}
	return (temp); // komut parçalarında sistemde değeri olan değişkenler varsa onların değerlerinin yerini yazılıp güncellendiği halini tutan değişkeni döndür
}

/*
  Expands environment variables in the string `str` using
  the environment variables `ev`.
  It modifies the original string in-place and returns it.

  Parameters:
    - str: Pointer to the string to be expanded.
    - ev:  Array of strings representing environment variables.

  Returns:
    - Pointer to the modified string with expanded environment variables.
*/
char	*expand_var(t_prompt *prompt, char *str, char **ev, int i) // stringin tamamında tespit edilen değişkenleri eğer değeri varsa değeri verilip yoksa eski haliyle aynısının döndürüldüğü fonksiyon
{
	int		q[4]; // tırnakları ve bazı indexleri tutacak değişkenler
	char	*sub_str; // eğer bir stringde $ ile bir değişken değeri istenmişse o değerin değişkene aktarılmış halini tutacak string

	(void)ev; // type casting yapılmış kullanılmayacak ama warning almamak için
	sub_str = NULL; // başlangıç değeri
	q[0] = 0; // tek tırnak içinde miyiz takibi, 0-hayır 1-evet
	q[1] = 0; // çift tırnak içinde miyiz, 0-hayır, 1-evet
	q[2] = i; // karakter indexi takibi
	q[3] = 0; // değişken adı uzunluğu vs gibi ekstra şeyler için
	if (ft_strcmp(str, "$") && ft_strlen(str) == 3) // bunu anlamadım bir özel durumuda değiştirmeyi atlamasını sağlıyor ama ne olduğunu bilmiyorum
		return (str); // str yi bir değişiklik yapmadan döndür
	sub_str = handle_expansion(prompt, str, q, sub_str); // str içinde $değişken_adı tespit edersen sistemde bu değişkenin değerini varsa bul yerine yaz ve yeni halini sub_str içine at
	if (sub_str) // eğer handle_expansion bir $değişken_adı tespit ederse ve bu değişkenin sistemde bir değerini bulup yerine koymuşsa boş bir string göndermemiştir buraya gir
		return (sub_str); // ve stringin değiştirilmiş değerler yerine yazılmış halini döndür
	else // eğer str içinde tespit edilen $değişken_adı olursa ve sistemde bir değeri bulunmazsa bu değişkenin o zaman NULL döner sub_str içinde
		return (str); // bu durumlarda da str nin ilk halini bir değişiklik yapmadan geri döndür
}

char	*handle_g_exitstatus(t_prompt *prompt, int i, char *str, char *sub_str) // eğer komut satırında $? değişkeni tespit edilirse onun alması gereken global g_exitstatus değerini yerine yazıp gönderen fonksiyon
{
	char	*nb; // g_exitstatus değişkeninin itoa ile stringe çevirilmiş halini tutan değişken
	int		len; // global değişkenin değerinin string olarak karakter uzunluğunu tutacak değişken

	len = 0; // başlangıç değeri 0
	nb = 0; // başlangıç değeri 0
	if (str[i + 1] == '?') // $ dan sonraki karakter ? ise
	{
		nb = grbg_itoa(prompt, g_exitstatus); // g_exitstatus un değeri yani önceki komut satırının sonlandığındaki çıkış kodunu integerdan stringe çevirip nb ye at
		if (!nb) // eğer nb boşsa
			return (NULL); // NULL döndür
		len = ft_strlen(nb); // sayı stringinin karakter uzunluğınu len e at
		sub_str = create_sub(str, i, nb, len); // sub_str içine str deki komut stringinde bulduğun $? karakterlerini g_exitstatus değeri ile değiştirilmiş halini at
		collect_grbg(prompt, sub_str); // güncellenmiş hali olan sub_str için de sistemde yer aç
	}
	return (sub_str); // istenilen çıkış kodunu yerine yazdığın stringi döndür
}

/*
q[2] // i
q[3] //len
q[4] //
*/
char	*handle_expansion(t_prompt *prompt, char *str, int q[4], char *sub_str) // $ karakterini bulan ve sonrasında kayda değer bir değişken ismi vs varsa ona değerini aktaracak fonksiyonları çalıştıran ve değerlerin yerine yazıldığı son halini döndüren fonksiyon
{
	while (str[q[2]]) // str nin sonuna gelene kadar
	{
		q[0] = (q[0] + (!q[1] && str[q[2]] == '\'')) % 2; // tek tırnak içinde miyim flagi
		q[1] = (q[1] + (!q[0] && str[q[2]] == '\"')) % 2; // çift tırnak içinde miyim flagi
		if (!q[0] && str[q[2]] == '$' && str[q[2] + 1] && str[q[2] + 1] != ' ') // tek tırnak içinde değilsem ve str de bulunduğum karakter $ ise ve dolardan sonraki karakter NULL ve space değilse yani $dan sonra değişken adı var mı diye emin olunuyor
		{
			if (str[q[2] + 1] == '?') // eğer $? ise
				sub_str = handle_g_exitstatus(prompt, q[2], str, sub_str); // exit_minishell.c de anlattım, g_exitstatus global değişkenimin değeri $? yerine yazılacak
			else // eğer $? dışındaki diğer durumlarsa yani $değişken_adı gibi gidip değşken_adı değişkeninin değerini bulup yerine yazacağız
			{
				q[3] = get_len_var(str, q[2] + 1); // $ karakteri sonrasını gönderip değişkenin adının kaç karakter olduğu hesaplanır ve q[3] e atılır
				sub_str = create_sub_var(str, q[2], prompt->envp, q[3]); // sub_str içine eğer $ karakteri sonrasında verilen isimde bir değişken varsa onun değerini stringin içinde yerine koyup son halini yazar
				collect_grbg(prompt, sub_str); // sub_str için de sistemde yer tahsis edilir
				if (sub_str == NULL) // eğer yer ayırırken ya da değişken değeri için çalıştırılan fonksiyonda değer bulmada vs bir sorun olduysa
				{
					sub_str = ""; // değişkenin içini boşalt
					break ; // döngüden çık
				}
			}
			str = sub_str; // değişkenin değeri verilmiş halini verilmemiş halini tutan değişkene at
		}
		q[2]++; // sonraki karaktere geç
	}
	return (sub_str); // stringin değişkenlerinin vs düzenlenmiş son halini döndür
}
