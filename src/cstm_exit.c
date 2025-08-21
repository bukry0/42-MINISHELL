/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cstm_exit.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 15:58:11 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 15:58:11 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
Exit Nedir?
exit = Shell’i sonlandırmaya yarayan builtin komut.
Shell (örneğin bash veya senin minishell’in) çalışırken, kullanıcı exit yazarsa:
Shell kapanır.
Çıkış kodu (exit status) sistem tarafından tutulur.
Çıkış kodu → genelde başarı (0) veya hata kodu (1–255) şeklinde kullanılır.

Çıkış Durumu (Exit Status)
Her Unix/Linux sürecinin bir dönüş değeri vardır.
Shell’de $? → en son çalıştırılan komutun çıkış kodunu tutar.
exit [n] → shell’i kapatırken çıkış kodunu n yapar. Örn:

exit 0
→ shell kapanır, çıkış kodu 0 (başarılı).

exit 42
→ shell kapanır, çıkış kodu 42.

Eğer argüman verilmezse:
exit
→ shell kapanır, en son kullanılan $? değerini kullanır.

Exit Kodlarının Kullanımı
0 → başarı (ör. komut sorunsuz çalıştı).
1 → genel hata.
126 → komut bulundu ama çalıştırılamıyor.
127 → komut bulunamadı.
130 → Ctrl+C (SIGINT) ile sonlandırıldı.
Senin minishell’in de bu kurallara benzer davranacak.

Örnek Kullanımlar

Tek başına çıkış
$ exit
→ Shell kapanır.

Belirli bir kod ile çıkış
$ exit 5
→ Shell kapanır, $?(g_exitstatus) = 5 olur.

Birden fazla argüman verilirse
$ exit 1 2
Gerçek bash çıktısı:
bash: exit: too many arguments
Shell kapanmaz.
Çıkış kodu 1 olur.

Sayı olmayan argüman
$ exit hello
Gerçek bash çıktısı:
bash: exit: hello: numeric argument required
Shell kapanır.
Çıkış kodu 255 olur.

Script içinde exit
Bir test.sh scripti yazalım:
#!/bin/bash
echo "Before exit"
exit 7
echo "After exit"
Çalıştırınca:
Before exit
Sonra script durur → $? = 7.
"After exit" yazılmaz çünkü exit scripti bitirmiştir.

Çıkış kodunun kalıcılığı
$ ls /notfound
ls: cannot access '/notfound': No such file or directory
$ echo $?
2
$ exit
Burada exit → son $? değerini (2) kullanarak shell’i kapatır.

Notlar
exit shell’i kapatır → shell kapanınca tüm child process’ler de sonlanır (örn. senin minishell’inde fork’lanmış process’ler).
Çıkış kodları özellikle script yazarken çok önemlidir:
if [ $? -ne 0 ]; then ... fi gibi kontroller yapılır

*/

int	is_only_digits(char *s) // gönderilen string sadece nümerik karakterlerden mi oluşuyor diye kontrol eden fonksiyon
{
	size_t	i; // stringde gezinirken kullanılacak index

	i = 0; // başlangıç değeri 0
	while (ft_isdigit(s[i])) // eğer indexteki karakter nümerikse
		i++; // sonraki karaktere geç
	if (ft_strlen(s) == i) // eğer döngüden çıkma sebebi string sonuna gelmesi ise
		return (1); // bütün karakterler nümeriktir 1: true döndür
	return (0); // eğer bütün karakterler nümerik değilse buraya düşer 0: false döndür
}

int	cstm_exit(t_cmddat *cmd_data) // exit komutu girildiğinde programdan manuel olarak çıkış yapacak fonksiyon
{
	g_exitstatus = 0; // çıkış durumunun hatasız bir şekilde gerçekleşmesi göz önüne alınarak başlangıç değeri 0: başarılı
	if (get_len_arr(cmd_data->full_cmd) >= 3
		&& is_only_digits(cmd_data->full_cmd[1])) // eğer 2 den fazla argüman varsa [0]: exit [1]: 42 gibi istenilen bir g_exitstatus un alacağı değer ve 2. argüman nümerikse
	{
		print_err_msg("exit", "too many arguments"); // fazladan argüman girildiğinin mesajını ekrana yaz
		return (1); // 1: genel hata kodu döndür ve çık
	}
	ft_putstr_fd("exit\n", cmd_data->outfile); // output yönlendirilen yere exit komutunu yaz (sanırım history için bende anlamadım)
	if (cmd_data->full_cmd[1]) // eğer 2. argüman varsa
	{
		g_exitstatus = ft_atoi(cmd_data->full_cmd[1]); // exit code değeri bu 2. olarak girilen nümerik stringin integer a çevrilmiş hali olarak ayarlanır
		if (g_exitstatus < 0) // eğer değer negatifse
			g_exitstatus = 256 + g_exitstatus; // pozitif haline çevirilmek için 256 ile toplanır çünkü exit code negatif değerler almaz
	}
	if (get_len_arr(cmd_data->full_cmd) >= 2 && !g_exitstatus) // eğer komut sayısı 1 den fazlaysa yani exit den sonra devam ediyorsa ve exit code 0: başarılı durumunda ise
	{
		ft_putstr_fd("minishell: exit: ", 2); // 2: std error çıkışına hata durmunun kaynaklandığı yer yazılır
		ft_putstr_fd(cmd_data->full_cmd[1], 2); // 2: std error çıkışına hataya sebep olan argüman yazılır
		ft_putstr_fd(": numeric argument required\n", 2); // 2: std error çıkışına hatanın sebebi yazılır
		g_exitstatus = 2; // exit code : 2 olarak ayarlanır
	}
	exit_ms(g_exitstatus, cmd_data->prompt); // istenilen kodla programdan çıkışı yapacak fonksiyon açğırılır
	return (0); // fonksiyon başarılı bir şekilde biterse 0 döndür ve çık
}
