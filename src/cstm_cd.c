/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cstm_cd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 15:54:40 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 15:54:40 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
cd Komutu (Change Directory)
cd, shell’de çalışma dizinini değiştirmek için kullanılan bir builtin (yerleşik) komuttur.
Yeni dizine geçtiğimizde, shell’in PWD (Present Working Directory → şimdiki dizin) environment değişkeni güncellenir.
Önceki dizine dönmek için kullanılan OLDPWD environment değişkeni de her cd işleminde güncellenir.

Kullanım Şekilleri:
Hiç argüman olmadan:
cd
→ Kullanıcının HOME dizinine geçer (örn. /home/username).

Belirli bir dizine geçmek için:
cd /usr/local
→ Çalışma dizini /usr/local olur.

Önceki dizine geri dönmek için:
cd -
→ En son bulunduğun dizine geçer. Ayrıca dizini ekrana da yazar.

Geçersiz dizin verilirse:
cd /olmayan/dizin
→ No such file or directory hatası döner.

Yanlış kullanım (çok fazla argüman):
cd dir1 dir2
→ cd: too many arguments hatası verir.

Örnek Senaryolar;

cd (argümansız)
/home/user/project$ cd
/home/user$
Shell seni HOME dizinine gönderir.
PWD=/home/user
OLDPWD=/home/user/project

cd /etc
/home/user$ cd /etc
/etc$
PWD=/etc
OLDPWD=/home/user

cd -
/etc$ cd -
/home/user$
Konsolda /home/user yazdırılır.
PWD=/home/user
OLDPWD=/etc

cd /olmayan/dizin
/home/user$ cd /olmayan/dizin
cd: No such file or directory: /olmayan/dizin

Özetle: cd, environment değişkenlerini (PWD, OLDPWD, HOME) güncelleyen bir builtin komuttur.
Senin yazdığın kodda bu değişiklikler fonksiyonlara ayrılarak yapılmış (modüler bir yaklaşım).
*/

/* custom getenv for the parsed environment variables in the prompt struct */
char	*get_envp(t_prompt *prompt, char *name) // verilen isimdeki key environment değişkeninin value değerini döndüren fonksiyon
{
	int		i; // dizinin içinde gezmek için kullanılacak index değişken
	int		l; // aranacak envp adının uzunluğunu tutan değişken
	char	*str; // aranacak env adını tutacak değişken
	char	*env_var; // aranan environment değişkeninin değerini tutan değişken

	i = 0; // index değeri başlangıçta 0
	if (!name) // eğer aranması istenen env değerinin adı boş gönderilmişse
		return (NULL); // NULL döndürerek fonksiyondan çık
	str = grbg_strjoin(prompt, name, "="); // str içine aranan env değişkeni adını ve sonuna = karakteri koy örneğin "path=" gibi
	if (!str) // eğer str boşsa
		return (NULL); // NULL döndür ve çık
	l = ft_strlen(str); // aranan env değişkeni adının uzunluğunu döndür değerinin de bulunduğu string içinde arama yapabilmek için
	while (prompt->envp[i]) // bütün environment değişkenlerini gez
	{
		if (!ft_strncmp(prompt->envp[i], str, l)) // eğer elimizdeki env değişkeninin adı gönderilen isimle uyuşuyorsa
		{
			env_var = grbg_strdup(prompt, prompt->envp[i] + l); // istenilen env değişkeninin içeriği kadar hafızada yer aç ve içeriği kopyala
			return (env_var); // istenilen isimdeki ortam değişkeninin değerini döndür
		}
		i++; // sonraki ortam değişkeni stringine ilerle
	}
	return (NULL); // eğer gönderilen isimle eşleşen bir env değişkeni bulunamadıysa NULL döndür
}

void	modify_envp(t_prompt *prompt, char *name, char *insert) // verilen environment değişkenini istenilen yeni değeriyle güncelleyen fonksiyon
{
	int		i; // index değişken
	char	*str; // environment değişkeninin güncel değerini tutacak değişken

	i = 0; // index başlangıçta 0
	if (!prompt->envp || !insert || !name) // prompt içindeki evironment değişkeni boşsa, güncellenecek environment değişkeni adı boşsa ya da güncellenecek environment içeriği boşsa
		return ; // fonksiyondan çık
	str = grbg_strjoin(prompt, name, "="); // ortam değişkeni adının sonuna = ekleyip str içine at örneğin; str: "name="
	if (!str) // eğer str boşsa yani güncellenecek ortam değişkeninin adı yoksa
		return ; // fonksiyondan çık
	while (prompt->envp[i]) // bütün ortam değişkenlerini dolaş
	{
		if (!ft_strncmp(prompt->envp[i], str, ft_strlen(str))) // str içeriği ile aynı isimle başlayan environment değişkeni varsa
			prompt->envp[i] = grbg_strjoin(prompt, str, insert); // bu ortam değişkeninin içeriğini verilen insert değişkeni içeriğiyle değiştir, örneğin envp[i]: "name=insert" formatında olur
		//		{
		//			char *tmp = grbg_strjoin(prompt, str, insert);
		//   			if (!tmp)
		//      			return ;
		//    		collect_grbg(prompt, prompt->envp[i]); // eski stringi garbage list'e ekle
		//			prompt->envp[i] = tmp;
		//		}
		i++; // sonraki ortam değişkeni stringine ilerle
	}
}

int	go_home_dir(t_prompt *prompt) // HOME dizinine geçmek için cd ya da cd ~ komutları girildiyse HOME dizinine geçen fonksiyon
{
	char	*home_dir; // home dizinini string olarak tutacak değişken
	char	*cwd_before; // değişim yapmadan önceki dizini tutacak değişken (history gibi bir kullanım var dizinlerde de)
	char	*cwd_after; // dizinin değişimden sonraki halini tutar

	home_dir = get_envp(prompt, "HOME"); // home isimli env değişkeninin value değerini at
	if (!home_dir) // eğer home dizini değeri boşsa
		return (print_err_msg_lng("cd", "not set", "HOME")); // HOME dizininin ayarlanmadığını belirten hata mesajı yazdırlıp çıkılır
	cwd_before = (char *)getcwd(NULL, 0); // fonksiyona girildiğinde içinde bulunulan dizin tutulur
	collect_grbg(prompt, cwd_before); // bu eski dizin kaybolmaması için hafızada yer açılır
	modify_envp(prompt, "OLDPWD", cwd_before); // OLDPWD keyi ile bulunan env değişkeni başlangıçtaki dizinle güncellenir
	chdir(home_dir); // chdir (change directory) hazır fonksiyonu ile HOME dizinine geçilir
	cwd_after = (char *)getcwd(NULL, 0); // geçiş yaptığımız dizin sistemden kopyalanır
	collect_grbg(prompt, cwd_after); // bulunduğumuz dizin için hafızada yer ayrılır
	modify_envp(prompt, "PWD", cwd_after); // env değerleri içindeki o an bulunduğumuz dizini tutan PWD içine yeni bulunduğumuz dizin değeri ile güncellenir
	return (0); // fonksiyon sonunda 0 döndürüp çıkılır
}

int	go_back_dir(t_prompt *prompt) // cd - komutu girilirse history (OLDPWD) den bir önceki dizine geri geçilmesini sağlayan fonksiyon
{
	char	*old_dir; // bir önceki dizini tutan değişken
	char	*cwd_before; // fonksiyona girildiğinde içinde bulunulan değişken
	char	*cwd_after; // dizin değiştirildikten sonraki halini tutan değişken

	old_dir = get_envp(prompt, "OLDPWD"); // OLDPWD içerisindeki bir önceki dizin değerini alır
	if (!old_dir) // eğer prom başladığından beri hiç dizin değiştirilmediyse
		return (print_err_msg_lng("cd", "not set", "OLDPWD")); // daha öncesi için kayıtlı bir dizin olmadığını hata olarak yazıp çıkar
	printf("%s\n", old_dir); // önceki dizinin yolunu ekrana yazdır
	cwd_before = (char *)getcwd(NULL, 0); // şuanda bulunduğu dizini eski dizin olarak kaydedilebilmesi için tut
	collect_grbg(prompt, cwd_before); // hafıza da yer ayır değişmeden önceki dizini tutan değişkene
	modify_envp(prompt, "OLDPWD", cwd_before); // OLDPWD key içini dizinin değişmeden önceki hali ile güncelle
	chdir(old_dir); // history den bulduğun eski dizine geç
	cwd_after = (char *)getcwd(NULL, 0); // geçtiğin dizini yeni dizin olarak tut
	collect_grbg(prompt, cwd_after); // hafızada yer ayır şu an geçiş yaptığın dizine
	modify_envp(prompt, "PWD", cwd_after); // şuanki dizini tutan PWD key inin value değerini güncellenen yeni dizin ile değiştir
	return (0); // 0 döndür ve çık
}

int	cstm_cd(t_cmddat *cmd_data) // cd (dizin değiştirme) komutunun yapacağı işi manuel yapacak fonksiyon
{
	DIR		*dir_user; // girilen dizin gerçekten mevcut mu bunun değerini tutan flag
	char	*cwd_before; // dizinin değiştirilmeden önceki hali
	char	*cwd_after; // dizinin değiştirildikten sonraki hali

	if (cmd_data->full_cmd[1] && cmd_data->full_cmd[2]) // 2 dean fazla argüman girildiyse argv[0]: cd, argv[1]: /dizin ya da - ya da ~
		return (print_err_msg_lng("cd", "too many arguments", NULL)); // fazla argüman olduğunun hata mesajı yazılıp çıkılır
	if (cmd_data->full_cmd) // komut satırı boş değilse
	{
		if (!cmd_data->full_cmd[1]) // || (cmd_data->full_cmd[1][0] == '~' && !cmd_data->full_cmd[1][1])) // komut satırı cd ya da cd ~ ise gir (cd ~ ekledim inş yanlış olmaz) 
			return (go_home_dir(cmd_data->prompt)); // HOME directory e geçmek için fonksiyonu çalıştır ve çık
		else if (!ft_strcmp(cmd_data->full_cmd[1], "-")) // komut satırı cd - ise
			return (go_back_dir(cmd_data->prompt)); // bir önceki dizine geri döndüren fonksiyonu çalıştır ve çık
	}
	cwd_before = (char *)getcwd(NULL, 0); // şu an bulunduğu dizini eski dizini tutacak değişkene kopyala
	collect_grbg(cmd_data->prompt, cwd_before); // hafızada yer aç eski dizin değeri için
	modify_envp(cmd_data->prompt, "OLDPWD", cwd_before); // şuan olduğun dizini OLDPWD (bir önceki bulunulan dizin değeri) içine at
	dir_user = opendir(cmd_data->full_cmd[1]); // kullanıcının girdiği dizin geçerli bir dizin mi
	if (!dir_user || chdir(cmd_data->full_cmd[1]) == -1) // eğer dizin geçerli değilse ya da dizin değiştirmek için çağırılan fonksiyon değişim yapılırken hata verdiyse
		return (print_err_msg_lng("cd", "No such file or directory",
				cmd_data->full_cmd[1])); // verilen dizinin geçerli olmadığına dair hata mesajını ekrana yaz ve çık
	closedir(dir_user); // dizin geri kapatılır
	cwd_after = (char *)getcwd(NULL, 0); // dizinin güncellenen hali alınır
	collect_grbg(cmd_data->prompt, cwd_after); // güncellenen halini tutan değişken için yer açılır
	modify_envp(cmd_data->prompt, "PWD", cwd_after); // şuan bulunduğu dizini tutan PWD env değeri yeni geçilen dizin değeri le güncellenir
	return (0); // 0 döndür ve çık
}
