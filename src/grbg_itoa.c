/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   grbg_itoa.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:12:54 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:12:54 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	get_len(long n) // gönderilen sayının kaç basamaklı olduğunu döndüren fonksiyon
{
	size_t	len; // basamak uzunluğu tutacak değişken

	len = 0; // başlangıçta 0
	if (n < 0) // eğer sayı negatifse
	{
		len += 1; // basamak uzunluğunu bir arttır '-' karakteri için
		n = -n; // sayının pozitif halini al
	}
	while (n != 0) // sayı 0 olana kadar devam et
	{
		n /= 10; // sayıyı 10 a bölerek 1 basamak küçült
		len++; // basamak sayısını 1 arttır
	}
	return (len); // toplam basamak uzunluğunu döndür
}

static char	*fill_the_array(int len, char *dest, int n) // sayıyı stringe çevirme işleminin algoritmasının bulunduğu fonksiyon
{
	int	i; // aktarılacak string içinde dolaşmak için index

	i = 0; // 0 ile başla
	if (n < 0) // eğer sayı negatifse
	{
		dest[0] = '-'; // stringin ilk karakterine '-' koy
		n = n * (-1); // sayıyı pozitif haline çevir
		i = 1; // indexi 1 ilerlet
	}
	dest[len] = '\0'; // stringin son karakterine NULL koy
	len -= 1; // uzunluğu 1 eksilt 
	while (len >= i) // basamak sayısına gelene kadar ilerle
	{
		dest[len] = (n % 10) + '0'; // stringin son karakterine sayının 10 ilie bölümünden kalanı yani birler basamağının değeri + '0' karakterini koy
		n /= 10; // sayıyı 10 a böl yani birler basamağını yok et
		len--; // basamak sayısını 1 eksilt
	}
	return (dest); // stringi döndür
}

char	*grbg_itoa(t_prompt *prompt, int n) // gönderilen integer değişkeni stringe çevirip döndüren fonksiyon
{
	char	*dest; // string halini tutacak değişken
	size_t	len; // gönderilen sayınının kaç basamaklı olduğunu yani kaç karakter kullanılacağını tutacak değişken
	long	num; // verilen sayı üzerinde yapılacak değişkenler sebbeiyle sayının ilk değerini kaybetmeyecek değişken

	if (n == -2147483648) // integer min değeri ise girilen sayı
		return (dest = grbg_strdup(prompt, "-2147483648")); // direkt string halini döndür
	if (n == 0) // eğer gönderilen sayı 0 ise
		return (dest = grbg_strdup(prompt, "0")); // direkt sıfır karakterini string olarak döndür
	num = n; // gönderilen sayının değerini kopyala
	len = get_len(num); // sayının basamak sayısını al
	dest = get_grbg(prompt, len + 1, sizeof(char)); // sayının tutacağı karakter sayısı + NULL kadar yer ayır
	if (!dest) // eğer yer ayırmada sorun oluştuysa
		return (NULL); // NULL döndür ve çık
	dest = fill_the_array(len, dest, num); // sayıyı stringe çevirecek asıl işi yapan fonksiyon
	return (dest); // sayının string halini döndür
}
