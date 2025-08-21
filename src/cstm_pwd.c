/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cstm_pwd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:00:20 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:00:20 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*

Pwd Nedir?
pwd = Print Working Directory, Terminalde şu anda bulunduğun dizinin tam yolunu gösterir.
Shell’in çalışma alanını anlamak için kullanılır. Örn;

pwd
Çıktı:
/home/user/projects/minishell

Çalışma Mantığı
Her shell oturumunun bir mevcut dizini (current working directory) vardır.
Bu dizin işletim sistemi tarafından takip edilir ve getcwd() (get current working directory) fonksiyonu ile alınır.
Senin minishell’de cstm_pwd() fonksiyonun genellikle şu şekilde işler:
getcwd(buffer, size) ile geçerli dizini alır.
Bunu stdout’a (veya redirect edilmiş outfile’a) yazar.
En sona newline ekler.
Başarıyla çalıştıysa 0 döner.

Senin Minishell’deki pwd
Argüman kabul etmez. (Yani pwd → çalışır, ama pwd abc diye yazarsan bash’te hata verir, senin shell’in de büyük ihtimal görmezden gelir.)
Çıktı mutlaka absolute path olur. (örn. /usr/local/bin, . veya .. yazmaz)

Çıkış Durumu (Exit Status)
Başarılıysa → 0
Eğer getcwd() hata verirse (örneğin bulunduğun dizin silinmişse) → 1 veya hata kodu döner.

Redirect ile Kullanımı
Tıpkı echo’da olduğu gibi, çıktı yönlendirilebilir.

pwd > path.txt
Çıktı: terminal yerine path.txt dosyasına yazılır.

Örnekler

Basit kullanım
pwd
Çıktı:
/home/user

Alt dizinlere inme
cd projects/minishell
pwd
Çıktı:
/home/user/projects/minishell

Symbolic link durumu
cd /tmp
pwd
Çıktı:
/tmp

Eğer /tmp bir symlink olsaydı, genelde çözümlenmiş gerçek yol yazılır.
Bash’te pwd -P (physical) vs pwd -L (logical) ayrımı vardır.
Ama senin minishell’de sadece getcwd() → yani fiziksel yol gösterilir.

Redirect kullanımı
pwd > current_path.txt
cat current_path.txt
Çıktı:
/home/user/projects/minishell

Hatalı durum (ileri seviye)
Bulunduğun dizini başka bir terminalden silersen:
rmdir /home/user/projects/minishell
pwd
Gerçek bash’te hata döner:
pwd: error retrieving current directory: getcwd: cannot access parent directories: No such file or directory
Senin minishell’de de getcwd() NULL dönerse hata mesajı basman gerekir.

Notlar
echo → argümanları terminale yazdırır.
pwd → argümansız, sadece çalışma dizinini yazdırır.
İkisi de builtin (shell’in kendi fonksiyonlarıdır), yani yeni bir process fork etmeden çalıştırılırlar.
pwd çok daha deterministictir: aynı ortamda çalıştırıldığında hep aynı sonucu verir.

*/

int	cstm_pwd(t_cmddat *cmd_data) // pwd (bulunduğu dizini yazma) komutunun işlevini yapan fonksiyon
{
	char	*cwd; // bulunduğumuz diziyi string olarak tutacak değişken

	cwd = getcwd(NULL, 0); // bu hazır foksiyon bize sistemde komutun çalıştırıldığı esnada hangi dizinde olduğumuzu string şeklinde döndürüyor
	collect_grbg(cmd_data->prompt, cwd); // işlem bittiğinde freelemek için garbage içine atıyoruz bu değişkeni de
	if (!cwd) // eğer çalışma dizini bize NULL olarak döndürüldüyse yani muhtemelen sistemsel bir hata
		return (1); // 1 döndürüp çıkıyoruz
	ft_putstr_fd(cwd, cmd_data->outfile); // bulunduğumuz dizini belirten stringi eğer redirect yoksa 0: stdout varsa da o dosyanın fd değeriyle o dosyaya yazdırıyoruz
	ft_putstr_fd("\n", cmd_data->outfile); // sonuna newline ekliyoruz
	return (0); // 0 döndürüp çıkıyoruz
}
