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
1: < (Input Redirection – Dosyadan Okuma)
Bir komutun stdin (0)’ini bir dosyadan okumasını sağlar.
Normalde komutlar input’u klavyeden (terminalden) alır, < ile bunu bir dosyaya bağlarsın. Örneğin;

cat < input.txt

Burada cat normalde terminalden bekler, ama < sayesinde input.txt dosyasını stdin olarak kullanır.
Yani cat dosyanın içeriğini ekrana basar.

2: << (Here-Document – Inline Input)
Bir komutun stdin’ini terminalde girdiğin satırlardan alır, ta ki sen belirli bir “bitirme kelimesi” (delimiter) girene kadar. Örneğin;

cat << END
Merhaba
Dünya
END

Burada cat komutu input’u kullanıcıdan alıyor.
Sen satır satır yazıyorsun (Merhaba, Dünya), ta ki END kelimesini girene kadar.
cat tüm bu yazdıklarını stdin gibi alıyor ve ekrana basıyor.
Yani << aslında bir geçici dosya gibi davranıyor ama terminalden veri alıyor.

3: > (Output Redirection – Dosyaya Yazma, Üzerine Yazma)
Bir komutun stdout (1) çıktısını bir dosyaya yönlendirir. Eğer dosya varsa üzerine yazar, yoksa yeni dosya oluşturur. Örneğin;

ls > files.txt

Normalde ls çıktıyı ekrana basar. > ile files.txt dosyasına kaydeder.
Dosya zaten varsa eski içerik silinir, yeni liste yazılır.

4: >> (Append Output Redirection – Dosyaya Ekleme)
Bu da stdout’u bir dosyaya yönlendirir ama üzerine yazmaz, sona ekler. Örneğin;

echo "hello" >> log.txt

echo çıktısını log.txt dosyasının sonuna ekler. Dosya yoksa yeni oluşturur.
Bir log tutma senaryosunda çok kullanılır çünkü eski içerik silinmez.

Özetle;
< → Dosyadan oku
> → Dosyaya yaz (üzerine yaz)
>> → Dosyaya ekle
<< → Terminalden satır satır oku, belirli bir kelime gelene kadar

*/


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

int	get_flags(int type, int file_access_type) // input ya da output redirectionları var mı bunun tespitini yapan fonksiyon
{
	if (file_access_type == 0) // eğer file girişi tespit edilmesi istenilen input yönlendirmesi ise bu değişken 0 gönderilir
	{
		if (type == 1 || type == 2) // eğer redirection tipi 1: < ya da 2: << ise
			return (O_RDONLY); // sadece input üzerinden çalışacağı için read only modu yeterli olacaktır
		if (type == 3) // eğer redirection tipi 3: > ise
			return (O_WRONLY | O_CREAT | O_TRUNC); // içine yazı da yazılması gerektiği için write only ve onun dışında da eğer yazılması gereken dosya yoksa açabilmemiz için creat ve eğer file varsa yazmadan önce içini boşaltmamız için trunc da gönderiyoruz
		if (type == 4) // eğer redirection tipi 4: >> ise
			return (O_WRONLY | O_CREAT | O_APPEND); // içine yazı yazılabilmesi için write only eğer yazılacak dosya yoksa açılması için creat ve dosya zaten varsa dosyanın tamamını silip yazmak yerine eski metinin sonuna yazmak için append
	}
	else if (file_access_type == 1) // eğer output yönlendirmesi yapıldıysa değişken 1 olarak gönderilir
	{
		if (type == 1 || type == 2) // eğer redirection tipi 1: < ya da 2: << ise
			return (0); // 0 döndürülür hiçbir izin verilmemiş demek sanırım zaten sadece okunacak...
		if (type == 3 || type == 4) // eğer redirection tipi 3: > ya da 4: >> ise
			return (0644); // 0644 döndürülür bu da sanırım bütün izinleri vermek demekti hatta yukarıdaki return değerleri ile aynı muhtemelen çünkü orada da byte kaydırma ile değerler oluşturuluyordu O_WRONLY | O_CREATE | O_APPEND mesela 644 oluyordu gibi bir şeydi tamda hatırlamıyorum vaktim kalırsa araştıracağım...
	}
	return (0); // hiçbir şarta girmediyse 0 döndür
}

int	open_fd_redir(t_prompt *prompt, t_cmddat *cmd_struct, int i, int type) // istenilen redirection a uygun modda istenilen dosyaların açılmasını sağlayan fonksiyon
{	
	int	io_flags[2]; // input ya da outputa yönlendirilmiş bir file var mı bunları tutan flagler 0: inputa 1: outputa olan yönlendirmeleri tutuyor

	io_flags[0] = get_flags(type, 0); // input flaginin tespitini yapan fonksiyona gönderilir
	io_flags[1] = get_flags(type, 1); // output flaginin tespitini yapan fonksiyon çağırılır
	get_rid_quotes(prompt); // tırnakları temizleyen fonksiyon çağırılır zaten gerekli tırnaklara göre bölündüğü için artık karakter olarak tırnakların tokenlerin başında sonunda kalmaya devam etmesine gerek yok
	if (type == 1) // eğer redirection tipi 1: < ise
		cmd_struct->file_open = open_file(cmd_struct->full_cmd,
				i, &cmd_struct->infile, io_flags); // file_open içine istenilen redirection tipine uygun olan modlarda istenilen dosya açılır ve dosyanın açık olduğunu belirten 1 değeri flag olarak atanır
	else if (type == 2) // eğer redirection tipi 2: << ise
		launch_heredoc(prompt, cmd_struct, i); // heredoc çalıştırması için fonksiyon çağırılır, diğerleriden farklı olarak bu redirection ile ekrana yazılan girdiyi kullanıcının belirlediği değil kendi açtığımız geçici bir dosya içinde tutar sonra istenilen yere aktarıp bu geçici dosyayı kapatırız yani diğer redirect lere göre daha farklı bir işlem gerekir bu sebeple sadece open_file fonksiyonuna göndermek yeterli değildir
	else if (type == 3) // eğer redirection tipi 3: > ise 
		cmd_struct->file_open = open_file(cmd_struct->full_cmd,
				i, &cmd_struct->outfile, io_flags); // file_open içine istenilen redirection tipine uygun olan modlarda istenilen dosya açılır ve dosyanın açık olduğunu belirten 1 değeri flag olarak atanır
	else // redirection tipi 1 2 ya da 3 değilse zaten geriye 4: >> kalıyor
		cmd_struct->file_open = open_file(cmd_struct->full_cmd,
				i, &cmd_struct->outfile, io_flags); // file_open içine istenilen redirection tipine uygun olan modlarda istenilen dosya açılır ve dosyanın açık olduğunu belirten 1 değeri flag olarak atanır
	return (0); // fonksiyon biterken 0 döndür
}
