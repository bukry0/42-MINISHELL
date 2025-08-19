/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envp_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:02:01 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:02:01 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* no garbage collection for any functions in this file as they do not leak */

char	*get_path_cmds(t_cmddat *cmd, char **ev) // verilen komutun çalıştırılabilir tam yolunu bulmak, örn: "ls" girildiğinde /bin/ls şeklinde döndürmeyi sağlayan fonksiyon
{
	char	*path; // bulnacak tam yolu tutacak değişken
	int		i; // index

	i = 0; // başlangıçta 0
	if (get_builtin_nbr(cmd) != 0) // eğer bu komut bir builtin ise (echo, cd, export vb) o zaman shell içinde zaten implement edilmiş olur
		return (NULL); // builtinler için dosya yolu aranmaz NULL döner
	if (!access(cmd->full_cmd[0], 1)) // eğer full_cmd[0] zaten geçerli bir yol ise (./a.out ya da /bin/ls gibi) access kontrolü yapılır 1: W_OK anlamına gelir
		return (ft_strdup(cmd->full_cmd[0])); // bu erişilebilir yolu direkt bellek tahsisi de yapıp döndürürüz
	while (cmd->full_cmd[i]) // eğer direkt verrilen yol erişilebir değilse erişilebilir yolu bulmak için PATH değişkenine kadar ilerlet
	{
		path = get_path(cmd->full_cmd[i], ev, 0); // istenilen komutu alıp dosya dizine üzerine ekleyerek erişilebilir mi diye kontrol ediyor
		if (path) // eğer geçerli yol bulunduysa
			break ; // döngüden çık
		i++; // sonraki ortam değişkenine ilerle
	}
	return (path); // bulunan geçerli dosya dizinini döndür
}

/* Get the full path of the command by searching through the PATH variable:
Searches through the PATH variable to find the full path of
the command (where it is executed),
returns the full path (including the command) if found, or
NULL if the command is not in any of the specified paths. */
char	*get_path(char *cmd, char **ev, size_t i) // prompt süreci boyunca o an bulunduğumuz dosya yolunda gönderdiğimiz komuta izin olup olmadığını tespit edecek fonksiyon
{
	char	**all_paths; // ana dizinden itibaren dosya yollarının tamamını dizi olarak tutan değişken
	char	*path; // o an bulunulan dosya dizinini ana dizinden itibaren yol olarak içerecek değişken
	char	*path_part; // bulunulan dizine gelene kadar olan dosyaların tek tek parçalanmış hallerini tutan değişken

	while (ev[i] && ft_strnstr(ev[i], "PATH", 4) == 0) // env değişeni sonuna gelmediysen ve env içeriği PATH karakterleriyle eşleşmiyorsa
		i++; // bir sonraki ortam değişkeni değerine ilerle
	if (i == get_len_arr(ev)) // eğer env değişeninin sonuna geldiğimi için döngü bittiyse
		return (NULL); // NULL döndür ve çık
	all_paths = ft_split(ev[i] + 5, ':'); // "PATH=" karakterlerinden sonrasını : seperator ile parçalara ayır ve all_paths içine at (bazı yerlerde dosya yolları aşırı uzun ve bazılarının arasında / bazılarının arasında :/ şeklinde bölünmüş oluyorsu demek ki : ile bölünenler farklı tiplerde aynı dosya yolunu gösteren şeylermiş mesela :/bin/user/...:/bin/user_name/... gibi gibi uzantılar oluyordu bellki gözünde canlanmıştır vaktim olursa bunu da detaylı araştırır yazarım)
	i = 0; // indec sıfırlanır
	while (all_paths[i]) // dosya yollarının bölünmüş hallerini tutan değişkenin sonuna kadar ilerlenir
	{
		path_part = ft_strjoin(all_paths[i], "/"); // bütün dosya yollarının sonuna / ekler mesela "/bin" -> "/bin/"
		path = ft_strjoin(path_part, cmd); // istenilen dizinde izin var mı diye araştırılması gereken komutu verilen PATH stringinin sonuan ekler örneğin; "/bin/ls"
		free(path_part); // bellekteki alanını boşaltır
		if (access(path, F_OK) == 0) // path içerisindeki dosya yolunda verilen komutun F_OK modunda (f_OK:dosya mevcut mu, R_OK:okuma izni var mı, W_OK: yazma izni var mı, X_OK: çalıştırma (execute) izni var mı) erişim izni vs var mı diye kontrol edilir eğer varsa 0 yoksa -1 döndürür access
		{
			free_split(all_paths); // dosya dizinin tamamını tutan all_paths için tahsis edilen bellek serbest bırakılır
			return (path); // sonuna eklenmiş komuta izini olan dosya dizinini döndürür
		}
		free(path); // eğer izin yoksa sonuna komutun eklenmiş hali olan değişkene ayrılan bellek serbest bırakılır
		i++; // sonraki dosya yoluna ilerle
	}
	free_split(all_paths); // bütün dosya yollarını tutan string dizisi için tahsis edilen belleği serbest bırak
	return (NULL); // NULL  döndür
}

/* Frees the memory allocated for an array of strings such as
the one created with ft_split. */
void	free_split(char **strs) // gönderilen string dizisine ayrılan alanları boşalt
{
	int	i; // index sayacı

	i = 0; // başlangıç değeri 0
	while (strs[i]) // stringlerin sonuna kadar ilerle
		free(strs[i++]); // elindeki string için ayrılan belleği serbest bırak
	free(strs); // stringlerin dizisi için ayrılan belleği de boşalt
}

void	add_last_cmd_to_envp(t_prompt *prompt) // son komuta ilerle ve o komutun ilk indexteki içeriğini ortam değişkenlerine "_=" olarak kaydeden fonksiyon
{
	int			l; // parçalara ayrılmış komutun kaç parçası olduğunu sayı olarak tutar
	t_cmddat	*cmd; // prompt içindeki komut içeriğini tutan t_cmddat tipindeki değişkenin üzerinde gerekli değişikliklerin yapılmış halini tutar

	l = 0; // başlangıçta uzunluk 0
	if (!prompt->cmd_list->data->full_cmd) // eğer komut listesi içinde full komutu tutan değişken boşsa
		return ; // fonksiyondan çık
	cmd = cstm_lstlast(prompt->cmd_list)->data; // gönderilen listenin sonuna ilerle ve son düğümünün de datasını cmd içine at
	l = get_len_arr(cmd->full_cmd); // tokenlerin toplam uzunluğunu l ye at
	if (l) // eğer l 0 değilse
		modify_envp(prompt, "_", grbg_strdup(prompt, cmd->full_cmd[0])); // ortam değişenini güncelle şununla: "_=cmd->full_cmd[0]", şuanda son cmd deyiz onin ilk komutunu yaz
}
