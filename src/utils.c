/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:27:19 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:27:19 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

size_t	get_len_arr(char **array) // array uzunluğunu bulur
{
	size_t	i; // sayaç 

	i = 0;
	if (array == NULL) // eğer boş array gönderildiyse girer
		return (0); // boyutu 0 döndürüp çıkar
	while (array[i]) // array in son stringine yanı boş olanına kadar ilerler
		i++;
	return (i); // array içinde kaç string varsa onu döndürür
}

int	ft_isspace(int c) // gönderilen karakterin space ya da tab benzeri atlanması gereken karakterler mi olduğunu tespit eder
{
	if (c == ' ' || (c >= 9 && c <= 13)) // space benzeri karakterlerse 1 döndürür
		return (1);
	else
		return (0); // diğer bütün karakterler için 0 döndürür
}

/*
delete a str from a string array
pos : position of the first str to del
count : how many str to delete
*/

char	**del_str(char **array, int pos, int count) // string dizisinden seçilen bir stringin çıkartılmasını sağlayan fonksiyon pos: silinecek stringin arraydeki indexi, count: dizideki toplam string sayısı
{
	int	i; // döngüde gezerken kullanılacak sayaç
	int	buff; // dizide sileceğimiz string den sonrasını aktarırken kullanılacak sayaç

	i = 0;
	while (i++ < count) // dizideki toplam string sayısının takibi yapılarak döngüye başlanır
	{
		buff = pos; // silinecek stringin dizideki indexi sayaca atanır
		while (array[++buff]) // dizinin sonuna gelene kadar silinecek diziden sonrasını kopyalayan döngü
			array[buff - 1] = array[buff]; // dizinin n+1. indexini n. indexe kopyalar
		array[buff - 1] = NULL; // dizinin biitiği anlaşılsın diye son indexe NULL koyulur
	}
	return (array); // dizinin istenilen stringinin silinmiş hali döndürülür
}

char	**add_str_to_arr(t_prompt *prompt, char **arr, char *str) // verilen string dizisinin sonuna yeni bir string daha ekleyen fonksiyon
{
	char	**new; // döndürülecek dizinin yeni halini tutacak değişken
	int		l; // dizinin uzunluğunu tutacak değişken

	l = get_len_arr(arr); // dizinin başlangıçta kaç string tuttuğu atanır
	new = get_grbg(prompt, l + 2, sizeof(char *)); // dizinin yeni halini tutacak değişkene yer ayrılır +2: sondaki NULL ve eklenecek yeni string için alan
	if (!new) // eğer yer ayrılırken hata olursa girilir
		return (NULL); // NULL döndürülür
	new[l] = grbg_strdup(prompt, str); // yeni dizinin son elemanına eklenmesi istenen string koyulur
	while (--l >= 0) // sondan başlayarak stringlerin yeni diziye aktarılması için döngüye girilir
		new[l] = grbg_strdup(prompt, arr[l]); // yeni dizinin içine eski dizideki stringler olduğu gibi kopyalanır
	return (new); // yeni dizinin son hali döndürülür
}

int	check_double_pipes(t_prompt *prompt) // girilen pipe shell komutlarında veya anlamına gelen || çift halde mi girilmiş yoksa farklı işlevi olan | olarak mı girilmiş bunun kontrolü yapılır
{
	int	i; // komutlar dizisinde gezmek için kullanılacak index

	i = 0;
	while (prompt->commands[i]) // tokenize edilmiş komutların içinde 0. indexten başlanıp sonuna kadar teker teker geziliyor
	{
		if (prompt->commands[i][0] == '|' && prompt->commands[i + 1] // komut zincirlerinde || pipeler yan yanaysa gir
			&& prompt->commands[i + 1][0] == '|')
			return (1); // double pipe durumu ise fonksiyon 1 döndürür
		i++;
	}
	return (0); // pipeler yoksa ve olanlar da tekse 0 döndür
}
