/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:23:47 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:23:47 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
  Removes unnecessary quotes from the commands stored in the prompt.
  It iterates over each command, trims unnecessary quotes from its arguments,
  and updates the prompt's command data accordingly.

  
  Parameters:
    - prompt: Pointer to the prompt containing the commands to be processed.

  Returns:
    - None.
*/

void	get_rid_quotes(t_prompt	*prompt) // artık input token edildiği için artık zaten parçalanmış stringlerde bulunan tırnakları temizlememizi sağlayan fonksiyon
{
	char		**temp; // temizlenmiş string dizisini tutacak değişken
	char		*trim_cmd; // tırnakları trimlenmiş stringi geçici olarak tutacak değişken
	int			i[2]; // i[0]: index, i[1]: komut sayısını tutuyor
	t_node		*current_node; // o an elimizde olan işlenecek düğümü geçici olarak tutar
	t_cmddat	*cmd_data; // elimizdeki düğümün içeriğini geçici olarak tutar

	current_node = prompt->cmd_list; // komut listesi current_node içine kopyalanıyor sonra içerisinde gezilmesi için
	while (current_node != NULL) // komut listesi sonuna kadar ilerle
	{
		cmd_data = current_node->data; // elimizdeki komut listesi düğümünün içeriğini cmd_data içerisine kopyala
		i[0] = 0; // indexin başlangıç değeri 0
		i[1] = get_len_arr(cmd_data->full_cmd); //komut satırının tamamının token sayısını tutuyor
		temp = get_grbg(prompt, i[1] + 1, sizeof(char *)); // parçalanmış komutların dizi halinde tutulması için gereken alan ayrılır
		if (!temp) // eğer alan ayrılırken sorun olduysa
			return ; // fonksiyondan çık
		while (cmd_data->full_cmd[i[0]]) // komutun tamamını tutan değişkenin sonuna kadar dolaşılır
		{
			trim_cmd = get_trimmed(prompt, cmd_data->full_cmd[i[0]], 0, 0); // komutun baştan ve sondan eğer tırnak karakterleri varsa onları siler
			temp[i[0]] = trim_cmd; // tırnakların silinmiş güncel halini temp komut dizisi içine olması gereken sırada yerleştirir
			i[0]++; // bir sonraki komuta ilerle
		}
		cmd_data->full_cmd = temp; // komut içeriğinin başlangıçtaki halini tutan değişken yerine güncellenmiş hali kopyalanır
		current_node = current_node->next; // komut listesinde bir sonraki komuta ilerlenir
	}
}

/*
  Removes unnecessary quotes from a str while preserving quoted substrings.
  It returns a new dynamically allocated string containing the trimmed result.

  Parameters:
    - s1: Pointer to the input string to be trimmed.
    - squote: Integer representing the state of single quotes in the string.
    - dquote: Integer representing the state of double quotes in the string.

  Returns:
    - Pointer to the dynamically allocated trimmed string.
*/

char	*get_trimmed(t_prompt *prompt, char const *s1, int squote, int dquote) // gönderilen stringde tırnak karakterleri dışındaki karakterleri kopyalayıp tırnaksız halini döndüren fonksiyon
{
	int		count; // malloc_len in hesaplayacağı silinecek tırnak sayısını tutuyor
	int		i[2]; // indexler, i[0]: input string üzerinde gezen index, i[1]: yeni string (trimmed) içine yazılacak index
	char	*trimmed; // komutların trimlenmiş halini tutacak değişken

	i[1] = -1; // başlangıç değeri -1 çünkü ++i[1] ile 0 olarak kullanılmaya başlanacak
	i[0] = 0; // başlangıç değeri 0
	count = malloc_len(s1); // s1 içinde kaç tane tırnak olduğunu hesaplar
	if (!s1 || count == -1) // s1 boşsa ya da s1 içinde hiç tırnak yoksa 
		return (NULL); // NULL döndür ve çık
	trimmed = get_grbg(prompt, ft_strlen(s1) - count + 1, sizeof(char)); // tırnakların trimlenmiş hali için hafızada yer açılır
	if (!trimmed) // eğer bellek ayrımada bir sorun olduysa
		return (NULL); // NULL döndür ve çık
	while (s1[i[0]]) // s1 sonuna kadar ilerle
	{
		squote = (squote + (!dquote && s1[i[0]] == '\'')) % 2; // eğer çift tırnak içinde değilsek, tek tırnak ' gördüğümüzde squote durumunu değiştirir (0 → 1 veya 1 → 0)
		dquote = (dquote + (!squote && s1[i[0]] == '\"')) % 2; // eğer tek tırnak içinde değilsek, çift tırnak " gördüğümüzde dquote durumunu değiştirir
		if ((s1[i[0]] != '\"' || squote) && (s1[i[0]] != '\'' || dquote) \
			&& ++i[1] >= 0) // tek tırnak içindeysek tek tırnakları tek içindeysek de çift tırnakları yaz, onun dışındaki tırnak karakterlerinde girme
			trimmed[i[1]] = s1[i[0]]; // kopyalanacak karakteri trimmed içine kopyala
		i[0]++; // sonraki indexe ilerle
	}
	trimmed[++i[1]] = '\0'; // trimlenmiş halinin sonuna NULL koy
	return (trimmed); // trimlenmiş halini döndür
}

/*
  Counts the number of unnecessary quotes in the string `str`.
  It checks the consistency of quotes and returns the count.

  Parameters:
    - str: Pointer to the input string to be checked.

  Returns:
    - Number of unnecessary quotes in the string.
*/

int	malloc_len(char const *str) // gönderilen stringde kaç tane tırnak işareti silineceğininin sayısını döndüren fonksiyon
{
	int	count; // sayaç
	int	squote; // tek tırnak takibi
	int	dquote; // çift tırnak takibi
	int	i; // index

	count = 0; // başlangıç değeri 0
	squote = 0; // başlangıç değeri 0
	dquote = 0; // başlangıç değeri 0
	i = 0; // başlangıç değeri 0
	while (str[i]) // stringin sonuna kadar ilerle
	{
		squote = (squote + (!dquote && str[i] == '\'')) % 2; // tek tırnak içindeysek 1 değilsek 0
		dquote = (dquote + (!squote && str[i] == '\"')) % 2; // çift tırnak içindeysek 1 değilsek 0
		if ((str[i] == '\"' && !squote) || (str[i] == '\'' && !dquote)) // çift tırnak içinde değilsek ve elimizde tek tırnak varsa ya da tek tırnak içinde değilsek ve elimizde çift tırnak varsa
			count++; // silinecek tırnak sayısını 1 arttır
		i++; // sonraki karaktere geç
	}
	if (squote || dquote) // eğer açılmış ama kapanmamış herhangi bir tırnak kaldıysa
		return (-1); // -1 döndür
	return (count); // silinecek tırnak sayısını döndür
}
