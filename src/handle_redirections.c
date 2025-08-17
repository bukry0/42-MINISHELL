/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_redirections.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:14:22 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:14:22 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
  Determines the type of redirection for the given string `str`
  and returns the corresponding type value.

  Parameters:
    - str: Pointer to the string representing a command or redirection.

  Returns:
    - 1 if the redirection is an infile-redirection ('<').
    - 2 if the redirection is a here_doc-redirection ('<<').
    - 3 if the redirection is a write to outfile ('>').
    - 4 if the redirection is an append to outfile ('>>').
    - 0 if no redirection type is detected.
*/
int	get_type(char *str) // gönderilen komut stringinde redirectionları ve tiplerini tespit eden fonksiyon
{
	int	type; // redirection ın tipini tutan değişken
	int	i; // string içinde dolaşmamızı sağlayacak değişken
	int	q[2]; // tırnak içinde olup olmadığımızı kontrol edecek flagler

	i = -1; // başlangıç değeri ++i kullanılan satırdan itibaren 0 olabilsin diye -1 verilmiş
	type = 0; // redirection olmadığında kullanılan değer başlangıç olarak verilmiş tespit edildiğinde redirection ın değeri atanacak
	q[0] = 0; // tek tırnak takibi yapan flagin başlangıç değeri
	q[1] = 0; // çift tırnak takibi yapan flagin başlangıç değeri
	while (str[++i]) // komut stringinin sonuna kadar ilerle
	{
		q[0] = (q[0] + (!q[1] && str[i] == '\'')) % 2; // tek tırnak içinde ise 1 değilse 0 değeri alır
		q[1] = (q[1] + (!q[0] && str[i] == '\"')) % 2; // çift tırnak içinde ise 1 değilse 0 değeri alır
		if (!q[0] && !q[1]) // eğer herhangi bir tırnak içerisinde değilsek (tırnak içindeyken redirection anlamı olan karakterleri görsek bile anlamlandırmamalıyız çünkü o sadece bir meti olarak ele alınacak)
		{
			if (ft_strlen(str) == 1 && str[i] == '<' ) // eğer str uzunluğu 1 ise ve karakter de < ise
				type = 1; // redirection tipini 1 olarak ayarla
			if (str[i] == '<' && str[i + 1] && str[i + 1] == '<') // eğer str içindeki 2 karakter << ise
				type = 2; // redirection tipi olarak 2 ayarla
			if (type == 0 && str[i] == '>') // eğer daha önde redirection tespit edilmediyse ve elindeki karakter > ise
				type = 3; // redirection tipini 3 olarak ayarla
			if (type == 3 && str[i + 1] && str[i + 1] == '>') // eğer redirection tipi 3: > tespit edilsiyse ve sonraki karakter de > ise
				type = 4; // redirection tipini 4 olarak göncelle
		}
	}
	return (type); // belirlenen redirection tipini tutan değişkeni döndür
}

/*
  Handles redirections within the commands
  stored in the prompt structure `ptr`.
  It iterates through the command list
  and processes each command to identify and handle redirections.

  Parameters:
    - ptr: Pointer to the prompt structure containing the command list.

  Returns: None
*/
void	handle_redir(t_prompt *ptr, int type) // redirection komutlarının tespitini ve sonrasındaki yapılacak işlemleri ayarlayan fonksiyon
{
	int			i; // komut zinciri içinde sırayla gezilmesini sağlayacak index
	t_node		*current_node; // içinde bulunulan komut düğümünü tutacak değişken
	t_cmddat	*cmd_data; // içinde bulunulan komut düğümünün içeriğini tutan değişken

	current_node = ptr->cmd_list; // komut listesinin ilk düğümünü atıyoruz current_node içine
	while (current_node != NULL) // son komuta gelene kadar ilerle
	{
		cmd_data = current_node->data; // cmd_data içine elimizdeki komut düğümünün içeriğini tutan değişkeni koyuyoruz
		if (cmd_data) // eğer bu değişkenin içeriği boş değilse
		{
			i = 0; // index i sıfırla
			while (cmd_data->full_cmd[i]) // cmd_data içindeki komutun tüm halini tutan değişkenin ilk tokeninden sonuna kadar ilerle
			{
				type = get_type(cmd_data->full_cmd[i]); // ilk komut parçasının tipini belirliyoruz örneğin, 0: redirection komutu yok, 1: <, 2: <<, 3: >, 4: >>
				if (type < 5 && type > 0) // eğer değişken tipi 1, 2, 3 ya da 4 ise
				{
					open_fd_redir(ptr, cmd_data, i, type); // redirection yapılan dosyanın open fonksiyonu ile açılmasını ve dosyanın kodunun cmd_data içindeki gerekli değişkene infile ya da outfile aktarılmasını sağlar
					cmd_data->full_cmd = del_str(cmd_data->full_cmd, i, 2); // redirection komutu için gerekli yönlendirmeler yapıldığı için artık redirection ı tutan komut parçası listeden silinebilir
					i -= 1; // eksilen liste elemanı yerine -1 +1 yapılır ki bir geriye kayan listeden eleman gözden kaçmasın
				}
				i++; // sonraki komut dizesine geçmek iin indexi 1 arttır
			}
		}
		current_node = current_node->next; // bir sonraki düğüme geç
	}
}

/*
if save_fd > 1 , it means that it's already open and we need to close it
*/

int	open_file(char **cmds, int i, int *save_fd, int io_flags[2] ) // redirection yapılmış bir dosyanın içerisine istenilen işlem için erişilebilmesi için open ile açan fonksiyon
{
	if (*save_fd > 1) // eğer save_fd değişkeni > 1 ise yani zaten bu dosya çoktan açılmış ise
	{
		if (close(*save_fd) == -1) // dosyayı kapatma fonksiyonunu çağır ve eğer kapatılırken bir hata oluşursa
			printf("Error while attempting to close a file"); // gerekli hata mesajını yazdır
	}
	if (cmds[i + 1]) // redirection dan sonraki komut dolu ise
	{
		if (io_flags[1] != 0) // eğer output flagi true ie
			*save_fd = open(cmds[i + 1], io_flags[0], io_flags[1]); // redirection dan sonraki komut (yani file ismi) ve O_RDONLY ve O_WRONLY modları ile file ı aç ve kodunu save_fd içine koy
		else
			*save_fd = open(cmds[i + 1], io_flags[0]); // output flagi dolu değilse sadece okuma modunda açılacak yani file ismi ve O_RDONLY ile open fonksiyonu çalıştırılır
		if (*save_fd == -1) // eğer open fonksiyonu fd değerini -1 döndürürse file lar açılırken hata oluşmuk demektir örneğin verilen isimde bir dosya bulunduğumuz dizinde yoksa vs
		{
			g_exitstatus = 1; // global olan sinyal hata durumunu tutan değişkene genel hata anlamı taşıyan 1 değeri atanır
			print_err_msg(cmds[i + 1], "No such file or directory"); // hatanın mesajı ekrana yazılır
			return (1); // 1 döndürülür
		}
	}
	else // redirection dan sonraki komut dolu değilse
		syntax_error(NULL, cmds[i + 1]); // bu bir sözdizim hatasıdır çünkü redirection sonrası bir file name verilmek zorunludur verilmediği için de syntax_error fonksiyonu ile hata fonksiyonu çağırılır
	return (0); // fonksiyon başarılı biterse 0 döndürülür
}

/*
Indicates that the file should be...
O_RDONLY: opened in read-only mode.
O_WRONLY: opened in write-only mode.
O_CREAT: created if it does not exist.
O_TRUNC: truncated (emptied) if it already exists.
O_APPEND: Indicates that data should be appended to the
 end of the file during writing.
00644: octal value used to specify the file permissions when creating it.
In this case, 00644 grants read and write permissions to
the file owner, and read permissions to other users.
*/

int	get_flags(int type, int file_access_type)
{
	if (file_access_type == 0)
	{
		if (type == 1 || type == 2)
			return (O_RDONLY);
		if (type == 3)
			return (O_WRONLY | O_CREAT | O_TRUNC);
		if (type == 4)
			return (O_WRONLY | O_CREAT | O_APPEND);
	}
	else if (file_access_type == 1)
	{
		if (type == 1 || type == 2)
			return (0);
		if (type == 3 || type == 4)
			return (0644);
	}
	return (0);
}

int	open_fd_redir(t_prompt *prompt, t_cmddat *cmd_struct, int i, int type) 
{
	int	io_flags[2];

	io_flags[0] = get_flags(type, 0);
	io_flags[1] = get_flags(type, 1);
	get_rid_quotes(prompt);
	if (type == 1)
		cmd_struct->file_open = open_file(cmd_struct->full_cmd,
				i, &cmd_struct->infile, io_flags);
	else if (type == 2)
		launch_heredoc(prompt, cmd_struct, i);
	else if (type == 3)
		cmd_struct->file_open = open_file(cmd_struct->full_cmd,
				i, &cmd_struct->outfile, io_flags);
	else
		cmd_struct->file_open = open_file(cmd_struct->full_cmd,
				i, &cmd_struct->outfile, io_flags);
	return (0);
}
