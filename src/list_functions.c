/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list_functions.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:18:37 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:18:37 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	cstm_lstiter(t_node *lst, void (*f)(void *)) // gönderilen listenin bütün elemanlarına f fonksiyonunu sırasıyla uygula
{
	if (!lst || !f) // liste ya da f fonksiyonu boşsa
		return ; // fonksiyondan çık
	while (lst) // listenin sonuna kadar ilerle
	{
		f(lst->data); // f fonksiyonu içine elindeki düğümün içeriğini gönder, fonksiyon işlevi bu düğüme uygulansın
		lst = lst->next; // bir sonraki node a ilerle
	}
}

t_node	*cstm_lstlast(t_node *lst) // gönderilen listenin son elemanına kadar ilerleyip son elemanını döndüren fonksiyon
{
	if (!lst) // eğer gönderilen liste boşsa
		return (NULL); // NULL döndür
	while (lst->next) // bir sonraki düğüm doluysa
		lst = lst->next; // bir sonraki düğüme ilerle
	return (lst); // elde edilen son düğümü döndür
}

int	cstm_lstsize(t_node*lst) // gönderilen linked list in kaç elemanı olduğunu döndüren fonksiyon
{
	int	i; // sayaç değişkeni

	i = 0; // başşlangıçta 0
	while (lst) // sonuna kadar ilerle
	{
		lst = lst->next; // bir sonraki düğüme ilerle
		i++; // her ilerlemede sayacı 1 arttır
	}
	return (i); // listenin boyutunu döndür
}

void	cstm_lstclear(t_node **lst, void (*del)(void *)) // gönderilen listenin tamamını silen fonksiyon
{
	t_node	*next; // düğümü silmeden önce bir sonraki düğümü kaybetmemek için tutacak olan değişken

	if (!lst || !del) // eğerlist boşsa ya da del fonksiyonu yoksa
		return ; // fonksiyondan çık
	while (*lst) // listenin sonuna kadar ilerle
	{
		next = (*lst)->next; // bir sonraki düğümün adresini tut
		cstm_lstdelone(*lst, del); // elindeki düğümü düğüm silen fonksiyona gönder
		*lst = next; // tuttuğun bir sonraki düğümü geç
	}
}

void	cstm_lstdelone(t_node *lst, void (*del)(void *)) // gönderilen düğümü silen fonksiyon
{
	if (!lst || !del) // eğer liste boşsa ya da del fonksiyonu tanımlanmamışsa
		return ; // fonksiyondan çık
	del(lst->data); // gönderilen düğümün içeriğini del fonksiyonu ile sil
	free(lst); // hafızada tahsis edilen yeri serbest bırak
}
