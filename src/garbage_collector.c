/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:11:12 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:11:12 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*	nmemb and size of new memory space (same as for ft_calloc) */
void	*get_grbg(t_prompt *prompt, size_t nmemb, size_t size) // nmemb*size kadar hafızada yer ayırıp bu yerin adresini döndüren fonksiyon
{
	void	*new; //gönderilen değişkenin hafızada yer ayrılmış halini tutacak değişken

	new = ft_calloc(nmemb, size);  // gönderilen değişken tipi boyutunda (nmemb = sizeof(char *) gibi) istenildiği kadar yer ayrılmasını sağlayan fonksiyon
	if (!new) //eğer yer ayırmada sorun olduysa
	{
		ft_putstr_fd("memory allocation error\n", 2); // ekrana memory allocation error yazılır
		g_exitstatus = 1; // exit code 1 (genel hata) olarak ayarlanır
		return (NULL); // ve NULL döndürülüp çıkılır
	}
	collect_grbg(prompt, new); // eğer yer ayırmada sorun olmadıysa, yer ayrılan değişken prompt içindeki garbage list e eklenir ki program kapatılırken yapılan bütün allocationlar beraber free lenebilsin
	return (new); // yer ayrılmış halini döndürüyoruz
}

/*	can also be used without get_grbg to collect malloc'ed space */
void	collect_grbg(t_prompt *prompt, void *new) // yer ayrılma işlemi yapılmış değişkenleri program sonunda beraber temizleyebilmek için prompt->grbg_lst içinde toplayan fonksiyon
{
	t_grbg	*node; // içinde ayrılan alanın adresini tutacak değişken
	t_grbg	*tmp; // garbage linked list içinde gezmemizi sağlayacak değişken
	t_grbg	**head; // garbage linked list in başlangıcının adresini tutacak değişken

	head = &(prompt->grbg_lst); // prompt içinde bulunan ve garbage linked listini tutan değişkenin adresini başlangıç adresi olarak head içine atıyoruz
	node = ft_calloc(1, sizeof(t_grbg)); // yeni eklenecek olan değişken için liste elemanı kadar yer ayırııyoruz
	if (!node) // eğer yer ayırmada sorun olduysa
	{
		ft_putstr_fd("memory allocation error\n", 2); //hatayı ekrana yazıyoruz
		g_exitstatus = 1; // exit code: 1 (genel hata) ayarlayıp
		return ; // fonksiyondan çıkıyoruz
	}
	node->ptr = new; // garbage linked list içine yeni gelecek elemanın adresini düğümün içeriğini tutan değişkene koyuyoruz
	node->next = NULL; // sonraki elemanın adresini tutacak değişkene de NULL atıyoruz çünkü şuan verilen değişken en sonda olacak
	if (!(*head)) // eğer listedeki ilk elemanın adresi yoksa yani ilk eleman şimdi eklenecek eleman olacaksa
	{
		*head = node; // ilk elemana şuan yer ayrılan elemanın adresi koyulur
		return ; // ve fonksiyondan çıkılır
	}
	tmp = *head; // eğer ilk eleman başka bir değişkense onu liste sonuna gidebilmek için basamak olarak kullanacağımız değişkene koyarız
	while (tmp->next) // sonraki elemanı boş olana kadar ilerle
		tmp = tmp->next; // sonraki eleman boş değilse ona geçer
	tmp->next = node; // listenin son elemanını bulduk bu elemanın nexti NULL dı onun yerine yeni elemanın adresini koyduk
	return ; // fonksiyon bitti çık
}

void	free_grbg(t_grbg *head) // hafızada alan açıldığı için garbage listesi içine alınan elemanlarının hepsinin hafızadaki yerlerini serbest bırakan fonksiyon
{ 
	t_grbg	*curr; // elindeki freelenecek adresi tutacak değişken
	t_grbg	*prev; // bir önceki adresi tutacak değişken

	curr = head; // linked listin ilk elemanını alarak başlıyoruz
	while (curr) // listenin sonuna kadar ilerle
	{
		if (curr->ptr) // eğer elindeki elemanın adresini tutan değişken doluysa
			free(curr->ptr); // boşalt
		prev = curr; // bulunduğun adresi diğer değişkenle de tut
		if (curr->next) // eğer bulunduğun düğümden sonraki düğüm boş değilse
			curr = curr->next; // sonraki düğüme geç
		else //eğer bulunduğun düğüm son düğümse
		{
			free(curr); // son düğümü de serbest bırak
			return ; // fonksiyondan çık
		}
		free(prev); // önceki düğümü tutan değişkeni de serbest bırak
	}
}
