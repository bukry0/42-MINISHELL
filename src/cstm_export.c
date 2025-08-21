/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cstm_export.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 15:59:14 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 15:59:14 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
export Komutu Nedir?
export, Linux/Unix shell’lerinde environment variable (ortam değişkeni) ayarlamak veya mevcut shell değişkenini environment’a eklemek için kullanılan bir komuttur.
Ortam değişkenleri, programların çalıştığı ortamı etkiler. Örneğin PATH, HOME, USER, PWD gibi değişkenler zaten tanımlıdır. Kullanım Şekilleri:

Yeni bir environment değişkeni tanımlamak:
export VAR=deger
Bu işlem VAR isminde bir environment değişkeni oluşturur ve değerini deger yapar. Örn:
export MYNAME=Ahmet
echo $MYNAME
# Çıktı: Ahmet

Var olan shell değişkenini environment’a aktarmak:
VAR=123
export VAR
Burada önce normal shell değişkeni tanımlanıyor (VAR=123).
export VAR ile environment’a ekleniyor. Artık başka programlar da VAR değişkenini görebilir.

Birden fazla değişken export etmek:
export VAR1=hello VAR2=world
echo $VAR1 $VAR2
# Çıktı: hello world

Export edilmiş değişkenleri görmek (parametresiz export):
export
Tüm environment değişkenlerini listeler. Format genellikle şu şekilde olur:
declare -x PATH="/usr/local/bin:/usr/bin"
declare -x HOME="/home/ahmet"
declare -x LANG="en_US.UTF-8"

Geçersiz identifier hatası (hatalı değişken adı):
export 1VAR=abc
# bash: export: `1VAR=abc': not a valid identifier
Değişken isimleri rakamla başlayamaz.
Harf, rakam, _ dışında karakter içeremez.

*/

int	cstm_export(t_cmddat *cmd) // export (yeni env değişkeni tanımlama) komutunun işini manuel yapacak fonksiyon
{
	int	r; // return değeri 0: başarılı 1: başarısız
	int	i; // komut argümanları içinde gezerken kullanılacak değişken
	int	id_len; // env değişkeninin isminin uzunluğunu tutacak değişken

	r = 0; // başlangıçta 0: başarılı çıkış
	i = 1; //export komutundan sonraki argümanlardan başlaması için başlangıç değeri 1
	id_len = 0; // bulunduğu env keyinin uzunluğu başlangıçta 0
	if (get_len_arr(cmd->full_cmd) == 1) // eğer argüman olarak sadece export komutu girildiyse
		return (print_export(cmd)); // bütün ortam değişkenlerini ekrana yaz ve çık
	if (!cmd->prompt->envp && get_len_arr(cmd->full_cmd) > 1
		&& get_len_id(cmd->prompt, cmd->full_cmd[i], 0)) // environment boş ve ilk export komutu geçerliyse
		cmd->prompt->envp = add_str_to_arr(cmd->prompt, cmd->prompt->envp,
				cmd->full_cmd[i++]); // yeni bir env değişkeni oluştur
	while (cmd->full_cmd[i]) // eğer daha fazla argüman varsa bu argümanların sonuna kadar ilerle
	{
		id_len = get_len_id(cmd->prompt, cmd->full_cmd[i], 1); // argümanda eklenmesi için verilen env değişkeninin key değerinin uzunluğu alınır
		if (id_len) // eğer key değeri uzunluğu 0 dönmediyse
			scan_envp(cmd, cmd->full_cmd[i], id_len); // env içerisinde zaten varsa güncelle yoksa yeni env ekle
		else // key uzunluğu 0 döndüyse
			r = 1; // return değerini 1: başarısız işlem yap ama çıkma devam et çünkü başka geçerli değerler varsa onlara gereken işlemleri yap
		i++; // sonraki argümandaki komuta geç
	}
	return (r); // r değerini döndür
}

int	print_export(t_cmddat *cmd) // sadece export olarak girildiyse komut bütün export edilmiş değişkenleri ekrana yazdıran fonksiyon
{
	int	i; // env değerleri içinde gezmek için kullanılcaka index

	i = 0; // başlangıç değeri 0: env[0]
	while (cmd->prompt->envp && cmd->prompt->envp[i]) // env dizisi boş değilse dizinin sonuna kadra ilerle
	{
		if (ft_strncmp(cmd->prompt->envp[i], "_=", 2)) // _ (underscore) isimli env değişkeni haricinde gir
			print_line_export(cmd, i); // gönderilen env değerini yazdır
		i++; // sonraki env dizisi elemanına ilerle
	}
	return (0); // fonksiyon bitiminde 0 döndür
}

/*	if			e.g. "TEST=hello"
	else if		e.g., "export test"
	else (l=0)	because of error or empty command */
void	print_line_export(t_cmddat *cmd, int i) // sadece export komutu girildiğinde ekrana yazdırılacak env değerinin 1 satırını doğru formatta yazdıran fonksiyon
{
	size_t	l; // env değişkeninin keyinin uzunluğunu tutacak değişken

	l = get_len_id(cmd->prompt, cmd->prompt->envp[i], 0); // env değişkeninin key değerinin uzunluğu l ye atılır
	ft_putstr_fd("declare -x ", cmd->outfile); // env öncesi her satırın başına yazılan stringi yazıyoruz
	if (l && l != ft_strlen(cmd->prompt->envp[i])) // eğer değişkenin hem ismi hem değeri varsa toplam_len!=isim_len
	{
		write(cmd->outfile, cmd->prompt->envp[i], l); // verilen env değerinin adını da ekrana yaz
		ft_putstr_fd("=\"", cmd->outfile); // = karakterini yaz ve tırnak aç
		ft_putstr_fd(cmd->prompt->envp[i] + l + 1, cmd->outfile); // tırnak içine value değerini yaz
		ft_putstr_fd("\"", cmd->outfile); // tırnağı kapat
	} // yazım formatının birebir aynısı şeklinde yazdık
	else if (l) // eğer key var ama value değeri yoksa
		write(cmd->outfile, cmd->prompt->envp[i], l); // sadece key değerini yaz
	else // bozuk string vs durumları
		ft_putstr_fd(cmd->prompt->envp[i], cmd->outfile); // ekrana yine yazıyoruz
	ft_putstr_fd("\n", cmd->outfile); // sonuna newline yazıp çıkıyoruz
}

int	get_len_id(t_prompt *prompt, char *str, int msg) // gönderilen env değerinin key kısmının uzunluğunu döndürecek ve key değeri isim formatına uygun mu kontrolü yapacak fonksiyon
{
	int		i; // env stringi içinde gezerken kullanılacak index
	int		e; // hata var mı diye bakan flag 
	char	*tmp; // string üzerinde geçici oynamalar yaparken kullanılacak değişken

	i = 0; // env stringin ilk karakterinden başla
	e = 0; // hata var mı flagi başlangıçta false
	while (str[i] && str[i] != '=') // env boş değilse içinde = karakteri bulana kadar ilerle
	{
		if (ft_isdigit(str[0])) // ilk karakter nümerikse
			e = 1; // hata flagini true yap env ismi rakamla başlayamaz 
		if (!ft_isalnum(str[i]) && str[i] != '_') // sadece harf, rakam ve _ kullanılabilir bunun dışında karakter varsa
			e = 1; // hata flagini true yap
		i++; // sonraki karaktere geç
	}
	if (e || !i) // hata flagi true olduysa ya da ilk karakterden itibaren str boşsa
	{
		if (msg) // eğer hata mesajı yazılacaksa != 0 olarak gönderilir
		{
			tmp = grbg_strjoin(prompt, "`", str); // başına ' koyarak env değerini birleştir garbage yardımıyla hafızada yer açarak
			tmp = add_to_str(prompt, &tmp, "'"); // env stringi sonuna da ' koy
			print_err_msg_lng("export", "not a valid identifier", tmp); // hatanın kaynağı olan env stringi yazılmıştı onun devamına da hata bulunan komut ismi ve hata sebebini de yaz
		}
		i = 0; // isim hatalı olduğu için döndürülecek uzunluğu 0 olarak ayarla
	}
	return (i); // uzunluğu döndür ve çık
}

/*	if VAR exists, replace VAR
	else if end of envp, add VAR */
int	scan_envp(t_cmddat *cmd, char *str, int id_len) // eklenmek için gönderilen değişkeni env de uygun yere ekler (zaten varsa günceller yoksa yeni env olarak ekler)
{
	size_t	i; // env değişkenleri içinde gezzerken kullanılacak index
	int		envp_id_len; // elimizdeki env değişkeninin key değerinin uzunluğu

	i = 0; // başlangıçta 0
	while (cmd->prompt->envp[i]) // env değişlenlerinin hepsini gez
	{
		envp_id_len = get_len_id(cmd->prompt, cmd->prompt->envp[i], 0); // elindeki env değişkeninin key uzunluğunu tut
		if (envp_id_len == id_len && !ft_strncmp(cmd->prompt->envp[i], str,
				id_len)) // eğer istenilen isimdeki değişken env içinde varsa 
		{
			if (ft_strchr(str, '=')) // ve bu mevcut key için yeni değer verilmişse
				modify_envp(cmd->prompt, grbg_substr(cmd->prompt,
						cmd->prompt->envp[i], 0, envp_id_len),
					grbg_strdup(cmd->prompt, str + envp_id_len + 1)); // env içindeki eşleşen keyi verilen yeni value değeriyle güncelle
			break ; // döngüden çık aradığımız key bulundu ve gerekli işlem yapıldı
		}
		else if (i == get_len_arr(cmd->prompt->envp) - 1) // eğer env değerlerinin sonuna kadar bakılmışsa ve key bulunmamışsa
		{
			cmd->prompt->envp = add_str_to_arr(cmd->prompt, cmd->prompt->envp,
					str); // yeni key ve value değerlerini env dizisi sonuna ekle
			break ; // döngüden çık
		}
		i++; // sonraki env değerine ilerle
	}
	return (0); // fonksiyon bitti 0 döndür ve çık
}
