/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_spaces.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:15:14 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:15:14 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
  Calculates and returns the special length of the string `str`,
  which represents the number of special characters that are not
  preceded or followed by a space.

  Parameters:
    - str: Pointer to the string to calculate the special length for.

  Returns:
    - The number of special characters that are not preceded or followed
	by a space in the string.
*/

static int	special_len(char *str, int count, size_t i) // gönderilen stringin kaç tane space karakterine ihtiyacı olduğunu hesaplayıp döndüren fonksiyon
{
	while (str[i] && str[i + 1]) // eğer string bitmediyse ya da sondan (NULL dan) bir önceki karakterinde değilsek
	{
		if (str[i + 1] && ((str[i] == '>' && str[i + 1] == '>')
				|| (str[i] == '<' && str[i + 1] == '<'))) // eğer stringin son karakterinde değilsek ve <<, >> operatörleri varsa
		{
			if (i > 0 && str[i - 1] != ' ') // eğer stringin ilk karakterinde değilse ve operatörden önceki karakter space değilse
				count++; // sayacı bir arttır çünkü birazdan buraya 1 karakter space eklenecek
			if (str[i + 2] && str[i + 2] != ' ') // eğer operatörden sonra string bitmiyorsa ve operatörden sonra space yoksa
				count++; // sayacı yine arttır çünki space eklenecek
			i += 1; // döngüden çıkmadan önce i yi arttır
		}
		else if (str[i] == '>' || str[i] == '<' || str[i] == '|') // DOUBLE DEĞİL TEK KARAKTERLİ OPERATÖRLERDEN BİRİSİ VARSA
		{
			if (i > 0 && str[i - 1] != ' ') // operatör stringin ilk karakterinde değilse ve öncesindeki karakterde space yoksa
				count++; // sayacı 1 arttır
			if (str[i + 1] && str[i + 1] != ' ') // eğer operatör string sonunda değilse ve sonrasındaki karakter space değilse
				count++; // sayacı bir arttır çünki bir space eklenecek stringe
			i++; // indexi bir arttır
		}
		i++; // indexi bir tane daha arttır çünki space eklendi ve onu geç
	}
	count++; // NULL için bir karakter daha ekle
	return (count); // eklenmesi gereken karakter sayısını döndür
}

static char	*allocate_mem(t_prompt *prompt, char *str) // space eklenmesi gerekecek durumlar için stringin güzenlenmiş son haline uyacak bşr yer ayrımı yapacak fonksiyon
{
	char	*new_str; // yer ayrılıp döndürülecek değişken
	int		len_str; // ayrılacak yerin boyutunu tutacak değişken

	len_str = 0; // başlangıç değeri
	new_str = NULL; // başlangıç değeri
	len_str = ft_strlen(str) + special_len(str, 0, 0) + 1; // düzenleme yapılmadan önceki halinin boyutu + eklenecek space karakteri sayısı
	new_str = (char *)get_grbg(prompt, len_str, sizeof(char)); // stringin oluşacak son halinin boyutu kadar yer ayıracak fonksiyon çağırılır
	if (!new_str) // eğer yer ayrılırken bir sorun çıktıysa
		return (NULL); // NULL döndürür
	return (new_str); // stringin son halinin boyutuna göre yer ayırtılmış stringi döndürür
}

static void	add_spaces_double(char *str, char **new_str, int *j, size_t *i) // >>, << gibi double karakterli operatörlere space eklenmesi gerektiğinde çağırılacak fonksiyon
{
	if (str[(*i) + 2] != ' ' && (*i) + 2 < ft_strlen(str)) // eğer double operatör olarak gönderilen karakterlerden sonrası space değilse ya da string sonu değilse
	{
		(*new_str)[(*j)++] = str[(*i)++]; // operatörün ilk karakterini yeni stringin ilk karakterine kopyalar
		(*new_str)[(*j)++] = str[(*i)++]; // operatörün ikinci karakterini yeni stringin 2. karakterine kopyalar
		(*new_str)[(*j)++] = ' '; // double operatör karakterleri sonrasına space ekler
	}
	else if ((*i) == 0 || str[(*i) - 1] != ' ') // eğer operatör stringin başında değilse veya öncesindeki karakter space değilse
	{
		(*new_str)[(*j)++] = ' '; // operatörler öncesinde bir space karakteri ekle yeni oluşturulacak stringe
		(*new_str)[(*j)++] = str[(*i)++]; // operatörün ilk karakterini ekle yeni stringde space den sonrasına
		(*new_str)[(*j)++] = str[(*i)++]; // operatörün ikinci karakterini ekle yeni stringe operatörün ilk karakterinden sonrasına
	}
	else // eğer eklenecek bir space e gerek yoksa
	{
		(*new_str)[(*j)++] = str[(*i)++]; // yeni stringe eski stringdeki peratörün ilk karakterini kopyalar
		(*new_str)[(*j)++] = str[(*i)++]; // yeni stringe eski strindeki operatörün ikinci karakterini kopyalar
	}
	return ;
}

static	void	add_spaces(char *str, char **new_str, int *j, size_t *i) // |, < vb gibi operatörlerin etrafına gerektiği durumlarda space ekler okunabilir bir kod formstıns gelebilmesi için
{
	if ((str[(*i)] == '>' && str[(*i) + 1] == '>')
		|| (str[(*i)] == '<' && str[(*i) + 1] == '<')) // eğer operatör >> ya da << gibi double olan ise 
		add_spaces_double(str, &(*new_str), &(*j), &(*i)); // add_spaces_double fonksiyonuyla etrafına ihtiyacı olan space eklenir
	else if ((str[(*i)] == '>' || str[(*i)] == '<'
			|| str[(*i)] == '|') && str[(*i) + 1] != ' ') // eğer tek karakterli operatörse ve sonrasındaki karakter space değilse sonrasına bir space eklenir, komut satırı parçalanırken okunabilirlik açısından
	{
		(*new_str)[(*j)++] = str[(*i)++]; // düzenlenmiş halini tutan stringe, eski stringde eklenecek space den sonrasının adresini ekler
		(*new_str)[(*j)++] = ' '; // string üst satırda bir karakter ileri kaydırıldı ve o açılan 1 karakterlik boşluğa bu satırda space eklendi
	}
	else if (i != 0 && (str[(*i)] == '>' || str[(*i)] == '<'
			|| str[(*i)] == '|') && str[(*i) - 1] != ' ') // eğer tek karakterli bir operatör öncesindeki karakter space değilse ve operatör ilk karakterde değilse
	{
		(*new_str)[(*j)++] = ' '; // yeni oluşturulacak stringde operatör öncesindeki karaktere space ekler
		(*new_str)[(*j)++] = str[(*i)++]; // eklediği space den sonraki karakterlere stringin geri kalanını aktarır
	}
	else
		(*new_str)[(*j)++] = str[(*i)++]; // stringde zaten eklenmesi gereken bir space hatası yok yeni stringe eskisi olduğu gibi aktarılır
	return ;
}

char	*handle_spaces(t_prompt *prompt, char *str, size_t i, int j) // space düzenlenmesi ihtimali olan karakterleri space düzenlemesi yapan fonksiyonlara aktaran fonksiyon
{
	char	*new_str;
	int		sgq; // single quote sayacı
	int		dbq; // double quote sayacı

	sgq = 0;
	dbq = 0;
	new_str = allocate_mem(prompt, str); // spacelerin düzenlenmiş halinin aktarılacağı stringe yer ayıracak fonksiyon çağırılır
	while (str[i]) // str stringinin sonuna kadar ilerle
	{
		sgq = (sgq + (!dbq && str[i] == '\'')) % 2; // eğerelimizdeki karakter tek tırnak karakteriyse sgq sayacını 1 arttır
		dbq = (dbq + (!sgq && str[i] == '\"')) % 2; // eğer elimizdeki karakter çift tırnak karakteri ise dbq sayacını 1 arttır
		if (!sgq && !dbq) // eğer tırnak içinde değilsek (tırnak içleri string olduğu için olduğu gibi alınmalı parçalamaya ya da anlamlandırmaya ihtiyaç duymayacağız)
			add_spaces(str, &new_str, &j, &i); // space eklenmesi gereken bir durum varsa ekleyecek fonksiyon çağırılır
		else // tırnak içindeysek
			new_str[j++] = str[i++]; // yeni aktaracağımız string karakteri içine eskisindeki karakteri aktarırız
	}
	new_str[j] = '\0'; // yeni string sonuna NULL koyulur
	return (new_str); // spacelerin düzenlenmiş hali olan yeni string döndürülür
}
