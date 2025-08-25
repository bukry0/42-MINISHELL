/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_var_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:09:25 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:09:25 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
Gets the lenght of the var to expand, starting from after the$
i is the position of the string */
ssize_t	get_len_var(char *str, int i) // verilen str stringinde i. indexten sonrasındaki karakterleri sayan; space, str sonu ya da tırnak görene kadar ve bu boyutu döndüren fonksiyon
{
	ssize_t	count; // boyut sayacı

	count = 0; // 0 ile başla
	while (str[i] && !ft_isspace(str[i]) && str[i] != '\"' && str[i] != '\'' ) // str sonunda değilsek ve bulunduğum karakter space vb., tek, çift tırnak karakterlerinden biri değilse
	{
		count++; // boyutunu hesapladığımız sayacı arttır
		i++; // sonraki karaktere geç
	}
	return (count); // verilen indexten sonrasında space ve tırnaklar ve str sonu gibi karakterler görene kadar ilerle ve bu ilerlemenin boyutunu döndür
}

/*
Searches for the value of the environment variable
specified by the string `str` within the array of environment variables `env`.
It returns a pointer to the value of the environment variable.

Parameters:
- str:          Pointer to the string representing the environment variable.
- var_exp_len:  Length of the environment variable to search for.
- env:          Array of strings representing environment variables.

Returns:
- Pointer to the value of the environment variable if found.
- Pointer to an empty string if the environment variable is not found.
*/
static char	*get_ptr_var(char *str, size_t var_exp_len, char **env) // gönderilen değişken adında bir değişken ortam değişkenleri içinde bulunursa o bulunan değişkenin sistemde aldığı değeri döndüren fonksiyon
{
	while (*env) // sistemden aldığımız environment ortam değişkenlerinin sonuna gelene kadar ilerle
	{
		if (!ft_strncmp(str, *env, var_exp_len) // eğer elimizdeki env stringi içinde komuttan aldığımız $değisken_adı ile aynı karakterler varsa ve bu değişken adından sonraki karaktter de = ise
			&& *(*env + var_exp_len) == '=')
			return (*env + var_exp_len + 1); // = karakterinden sonrasını döndür çünkü buradan itibaren aradığımız değişken adına sistemde verilmiş olan değeri yazıyor
		env++; // sonraki ortam değişkeni stringine geç
	}
	return (NULL); // eğer verilen isimde bir değişken bulamadıysan NULL döndür
}

/*
Creates a substring by expanding the environment
variable specified by the substring in the string
`str`. It constructs and returns a new string with the
 expanded environment variable.

Parameters:
- str: Pointer to the string containing the environment
variable to be expanded.
- i:   Index of the starting character of the environment variable substring.
- ev:  Array of strings representing environment variables.
- len: Length of the environment variable substring to expand.

  Returns:
    - Pointer to the newly created string with the expanded environment variable.
*/

// sıla export TEST=42; echo $TEST | cat
char	*create_sub_var(char *str, size_t i, char **ev, ssize_t len) // stringde tespit edilen değişkenin eğer atanmış bir değeri varsa onu yerine koyup stringin geri kalanıyla birleştirip gönderen fonksiyon
{
	char	*s1; // stringde değeri değiştirilecek değişkenden önceki karakterleri tutar
	char	*s2; // stringde değeri değiştirilecek olan değişkeni tutar
	char	*s3; // stringde değeri değiştirilecek olan değişkenden sonraki karakterleri tutar
	char	*expanded_tmp; // üstteki 3 stringi birleştirmek için basamak olarak kullanılır
	char	*expanded_str; // stringe değişkenin değerinin de verildiği ve birleştirildiği son halini tutar

	expanded_tmp = NULL; // başlangıç değeri NULL
	s1 = ft_substr(str, 0, i); //  stringin start olarak gönderilen karaktere kadar olan kısmını tutar
	s3 = ft_substr(str, i + len + 1, ft_strlen(str) - i - len); // stringin start+lenght yani doldurulması istenilen kısmının bittiği yerden sonrasındaki karakterleri tutar
	s2 = get_ptr_var(&str[i + 1], len, ev); // başlangıç karakterinden ($) bir sonraki karakteri ve tespit edilen değişken adı uzunluğunu gönderip değişken adının ve sistemdeki kayıtlı değerinin tespitini yapar ve s2 içine atar
	if (s2 == NULL) // sıla
		s2 = ft_strdup(""); // get_ptr_var NULL dönerse strdup içine direkt atmak seg fault oluyordu "" gönderiyorum
	else
		s2 = ft_strdup(s2);
	if (s2 != NULL ) // eğer aranan değişken ismine karşılık bir değer bulunduysa sistemde
	{
		expanded_tmp = ft_strjoin(s1, s2); // $ karakterinden önceki string ile dolar karakterinden sonraki değeri yenilenen değişken string birleştirilir
		expanded_str = ft_strjoin(expanded_tmp, s3); // az önce birleştirilen ikiliye yeni değeri verilen değişkenden sonraki string kısımları da eklenir
	}
	free(s1); // değişkenden öncesini tutan geçici string belleği serbest bırakılır
	free(s3); // değişkenden sonrasını tutan geçici string belleği serbest bırakılır
	if (s2 == NULL) // eğer değişkenin sistemde bir değeri yoksa ve NULL döndürüldüyse
		return (NULL); // stringin tamamı da NULL döndürülür
	else // eğer sistemde bir değeri varsa ve değişimi yapıldıysa
	{
		free(s2); // sistemdeki değerini tutan geçici değişkene tahsis edilen bellek serbst bırakılır
		free(expanded_tmp); // birleştirme işleminde basamak olarak kullanılan stringe tahsis edilen bellek serbest bırakılır
	}
	return (expanded_str); // stringe değişkenin değeride aktarılmış olan son birleşmiş hali geri döndürülür
}

char	*create_sub(char *str, size_t i, char *nb, ssize_t len) // g_exitstatus değerini $? komutu yerine yazan fonksiyon
{
	char	*s1; // $? karakterlerinden öncesini tutacak değişken
	char	*s3; // $? karakterlerinden sonrasını tutacak değişken
	char	*expanded_tmp; // bu stringleri birleştirirken basamak olarak kullanılacak değişken
	char	*expanded_str; // bütün stringlerin birleştirilmiş son halini tutan değişken

	s1 = ft_substr(str, 0, i); // $ karakterinin bulunduğu indexe kadar olan kısmı s1 içine kopyala
	s3 = ft_substr(str, i + len + 1, ft_strlen(str) - i - len); // $? karakterlerinden sonrasını s3 içine at
	expanded_tmp = ft_strjoin(s1, nb); // $ karakteri öncesini tutan stringin devamına ($?) yerine g_exitstatus değerini ekle
	expanded_str = ft_strjoin(expanded_tmp, s3); // bunun da devamına $? sonrasında kalan karakterleri birleştir
	free(expanded_tmp); // birleştirmede basamak olarak kullanılan değişkene ayrilan belleği serbest bırak 
	free(s1); // $ öncesini tutan geçici değişkene ayrılan alanı serbst bırak
	free(s3); // $ sonrasını tutan geçici değişkene ayrılan alaı serbest bırak
	return (expanded_str); // $? yerine değeri olan g_exitstatus koyulmuş halini tutan değişkeni döndür
}
