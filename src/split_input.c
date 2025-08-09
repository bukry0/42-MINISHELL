/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:26:38 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:26:38 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
Counts the number of words in the given string `str`,
considering a set of separators defined by `sep`.
It handles cases where words might be enclosed within single or double quotes,
ensuring they are not counted separately.
If there's a mismatch in quotes, it returns -1 to indicate an error.
*/
static int	ft_count_words_ms(const char *str, char *sep, int count, int i) // komut satırında girilen stringin space ve tırnaklara bölündüğünde kaç string parçası ortaya çıkacağını hesaplayan fonksiyon
{
	int	q[2]; // quote yani tırnakların sayısını tutması için 2 karakterlik sayı dizisi değişkeni

	q[0] = 0; // tırnak açılmış mı ya da açıldıysa kapanmış mı bunu tutar
	q[1] = 0; // eğer çift ya da tek bir tırnak kullanıldıysa o kullanılan karakteri tutar
	while (str[i] != '\0') // stringin sonuna gelene kadar ilerle
	{
		if (!ft_strchr(sep, str[i])) // eğer elimizdeki karakter seperator olarak gönderilen karakter değilse
		{
			count++; // yeni kelime gelecek onun başlangıcı olduğu için kelime sayısı 1 arttırılır
			while ((!ft_strchr(sep, str[i]) || q[0]) && str[i] != '\0') // saymaya başladığımız kelimenin sonunda değilsek ve eğer bir tırnak açıldıysa ve henüz kapanmadıysa ya da seperator bulduysan devam et
			{
				if (!q[1] && (str[i] == '\"' || str[i] == '\'')) // eğer kapanış tırnağı yoksa ve elimdeki karakter çift ya da tek tırnaksa
					q[1] = str[i]; // q[1] e bulduğun tırnak karakterini aktar, çift tırnağın tek tırnakla kapatılamaması için
				if (str[i] == q[1]) // eğer elindeki karakterle q[1] e atılan karakter aynıysa
					q[0] = (q[0] + 1) % 2; // q[0] sayısını bir arttır ve çift ya da tek olmasına göre ayırmak için 2 ile bölümünden kalanına eşitle, eğer elimizdeki tırnak sayısı çiftse false 0 (false) değeri alır, eğer elimizdeki tırnak sayısı tek kaldıysa 1 (true) değeri alır q[0]
				q[1] = q[1] * (q[0] != 0); // q[1] i  yani tırnak karakterini q[0] ın 1 (true) ya da 0 (false) olması durumuna göre çarpıp eğer açılan tırnak kapandıysa 0 ile çarpıp tırnak karakterini boşaltıyoruz q[1] içinden eğer henüz kapanış tırnağı bulunmadıysa q[1] içindeki tırnak karakterini 1 ile çarparak değerini koruyoruz ki kapanacağı ana kadar karakteri koruyalım ve aramaya devam edelim
				i++; // sayacı 1 arttır
			}
			if (q[0]) // eğer döngü bittiğinde kapanmamış bir tırnak kaldıysa 
				return (-1); // -1 hata döndür
		}
		else // eğer karakter seperator olan karakterse
			i++; // bir sonraki karaktere geç
	}
	return (count); // hesapladığımız kelime sayısını tutan değişkeni döndür
}

/*
	i[0] : character_index
	i[1] : start_index
	i[2] : word_index
*/
char	**ft_create_substrs(t_prompt *p, char **aux, char const *s, char *set) // set karakterine göre ve tırnaklara göre girilen komut satırını bölen fonksiyon
{
	int		i[3]; // index değişkenleri dizisi sırasıyla o an bulunulan karakterin, kelime başlangıç karakterinin ve kaçıncı kelimede olduğumuzu tutar
	int		q[2]; // tırnakları tutar, q[0] tek tırnak, q[1] çift tırnak

	q[0] = 0; // hepsinin başlangıç değeri 0
	q[1] = 0;
	i[0] = 0;
	i[1] = 0;
	i[2] = 0;
	while (s[i[0]]) // girilen komut satırı stringinin sonuna gelene kadar ilerle
	{
		while (ft_strchr(set, s[i[0]]) && s[i[0]] != '\0') // eğer ayırıcı (seperator) karakter elimizdeki karakterle aynııysa ve stringin sonunda değilsek
			i[0]++; // bir karakter atla
		i[1] = i[0]; // seperator karakterleri atladık o karakterin dışında bir karakter geçti elimize artık bu index bizim için bir kelime başlangıç indexi olacak o yüzden i[1] indexinde bu başlangıç konumunu tutuyoruz
		while ((!ft_strchr(set, s[i[0]]) || q[0] || q[1]) && s[i[0]]) // seperator olarak gönderilen karakterden farklıysa ya da herhangi bir açılmış tırnak varsa ve string sonunda değilsek
		{
			q[0] = (q[0] + (!q[1] && s[i[0]] == '\'')) % 2; // (eğer elimdeki karakter tek tırnaksa ve daha önce açılmış ama kapanammış bir çift tırnak yoksa) şartı doğruysa 1, değilse 0 + daha önce tek tırnak açıldıysa (açıksa 1 değilse 0) % 2 bu değerin 2 ile bölümünden kalan yani karışık bile görünse aslında tek tırnak açıksa henüz kapanmadıysa 1 açık değilse ya da açık olan bir tırnağı kapatacak tırnak şuan elimizdeyse 0 yapar q[0] değerini
			q[1] = (q[1] + (!q[0] && s[i[0]] == '\"')) % 2; // aynı şekilde çift tırnağı tutan q[1] değeri de henüz kapanmamış ama açılmış çift tırnak varsa 1, kapanmış ya da hiç açılmamışsa 0 olarak değeri verilir
			i[0]++; //bir karakter ilerle
		}
		if (i[1] >= (int)ft_strlen(s)) // eğer kelime başlangıcı olarak atanan index stringin tamamının uzunluğundan büyükse artık bölünecelk kelimeler bitmiş demektir
			aux[i[2]++] = NULL; // kelime dizisinin sonuna NULL koyulur
		else // eğer string sonunda değilsek yani hala devam edeceksek yeni kelimeler eklemeye
			aux[i[2]++] = grbg_substr(p, s, i[1], i[0] - i[1]); // elimizdeki kelimeye başlangıç karakterin indexi ve son karakterin indexinden ilkini çıkartıp doğru uzunlukta yer ayırıp o aralıktaki metini parçalanmış string dizisindeki yerine kopyalayan fonksiyona göndeririz
	}
	return (aux); // parçalanmış kod dizisini döndürürüz
}

/* Split the input read from the readline and put it into
	char		**commands; from the original prompt struct
*/
char	**split_input(char *str, t_prompt *prompt) // komut satırında girilen stringi spacelere ve tırnaklara göre anlamlı parçalara ayıran foksiyon
{
	char	**arr; // parçalanan komutları ayrı stringler halinde tutacak string dizisi değişken
	int		word_count; // parçalanmış halinin kaç farklı parçadan oluşacağını sayacak değişken

	if (!str) // eğer komut olarak boş bir string gönderildiyse
		return (NULL); // NULL döndür
	if (ft_isspace(*str)) // eğer ilk karakter space vb karakter ise
		str++; // sonraki karaktere geç, space i atla
	word_count = ft_count_words_ms(str, " ", 0, 0); // gönderilen komut satırını kaç parçaya böleceğimizi hesaplayan fonksiyona gönder
	if (word_count == -1) // eğer kelime sayısı olarak -1 döndürüldüyse
	{
		printf("Syntax error: unclosed quote in argument\n"); // kapanmamış bir tırnak var demektir ve ona uygun hata mesajı ekrana yazdırılır
		return (NULL); // NULL döndürülür
	}
	arr = get_grbg(prompt, word_count + 1, sizeof(char *)); // oluşturduğumuz string dizisine hesapladığımız alan kadar bellekte yer açıyoruz
	if (!arr) // eğer alan açarken bir sorun oluştuysa
		return (NULL); // NULL döndür
	arr = ft_create_substrs(prompt, arr, str, " "); // alt stringlere bölünüp string dizisine sırasıyla yerleştirilmesini sağlayan fonksiyon çağırılır
	arr[word_count] = NULL; // bölünmüş stringlerin bulunduğu dizinin sonuna NULL koyarız
	return (arr); // oluşturulan string dizimizin son hali döndürülür
}
