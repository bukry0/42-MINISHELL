/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:20:10 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:20:10 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	add_node_to_list(t_prompt *prompt, t_node **head, t_cmddat *data) // verilen linked listeye yeni bir düğüm daha ekleyen fonksiyon
{
	t_node	*new_node; // yeni eklenecek node u içinde tutacak değişken
	t_node	*current; // basamak olarak kullanılacak değişken

	new_node = get_grbg(prompt, 1, sizeof(t_node)); // yeni düğüm için bellekte alan ayrılır
	if (!new_node) // eğer alan ayrılırken bir hata oluştuysa
		return ; // fonksiyondan çık
	new_node->data = data; // yeni düğümün tutması gereken içeriği düğümün data değişkenine koyarız
	new_node->next = NULL; // bir sonraki düğümün adresini tutacak değişkene şimdilik NULL koyarız
	if (*head == NULL) // komut listesinin ilk elemanı boşsa
	{
		*head = new_node; // bize eklememiz için verilen düğümü en başa koyarız
	}
	else // eğer verilen linked list boş değilse
	{
		current = *head; // bu listenin başını current değişkeni içine koyarız şimdilik tutması için
		while (current->next != NULL) // listenin sonuna gelene kadar ilerle
			current = current->next; // bir sonraki değişkene eşitleriz current ı döngü bittiğinde listenin son değişkenini tutuyor olacak
		current->next = new_node; // son değişkenden sonra gelen boş olan değişkene yeni ekleyeceğimiz değişkeni koyarız
	}
}

int	ft_listsize(t_node *lst) // gönderilen linked list in uzunluğunu hesaplar
{
	size_t	i; // sayaç olarak kullanılacak index

	i = 0; // 0 ile başlarız
	while (lst) // listenin sonuna gelene kadar ilerle
	{
		lst = lst->next; // bir sonraki değişken değerini tut
		i++; // sayacı bir arttır
	}
	return (i); // sayacın son hali listenin uzunluğuna eşit olur
}
