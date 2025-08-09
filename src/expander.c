/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:10:19 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:10:19 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


/*
env nedir?
env kelimesi, “environment variables” (çevre değişkenleri) anlamına geliyor. Çevre değişkenleri, işletim sisteminin ve programların
birbirleriyle bilgi paylaşmasını sağlayan anahtar = değer çiftleridir. Örneğin;

PATH=/usr/local/bin:/usr/bin:/bin
HOME=/home/kullanici
USER=kullanici
SHELL=/bin/bash

Burada;
PATH: Program çalıştırma yollarını tutar. Terminalde ls yazdığında, ls komutunun hangi klasörde olduğunu bulmak için PATH kullanılır.
HOME: Kullanıcının ana dizini.
USER: Kullanıcı adı.
SHELL: Varsayılan kabuk.

Sen programını main(int argc, char **argv, char **env) şeklinde yazdığında 3. parametre olan env, işletim sisteminin sana bu değişkenleri
string array olarak veriyor. Yani env aslında şuna benziyor;
env[0] = "PATH=/usr/local/bin:/usr/bin:/bin";
env[1] = "HOME=/home/kullanici";
env[2] = "USER=kullanici";
env[3] = NULL; // Bittiğini gösterir

$ işaretleri ne işe yarıyor?
Shell dünyasında $ değişken okuma operatörüdür.
$HOME → HOME değişkeninin değerini getirir.
$USER → kullanıcı adını getirir.
$PATH → PATH değişkenini getirir.
$? → Son çalıştırılan komutun çıkış kodunu (exit status) verir.
Örneğin, bash;
echo $HOME
# çıktı: /home/kullanici

echo $USER
# çıktı: kullanici

ls asdf
# ls: cannot access 'asdf': No such file or directory
echo $?
# çıktı: 2   (hata kodu)

Shell neden $ gördüğünde değiştirme yapar?
Buna variable expansion denir (değişken genişletme).
Shell, $ ile başlayan ifadeleri ilgili değişkenin değerine çevirir. Örneğin, Bash;

echo "Merhaba $USER, evin: $HOME"
Shell bunu çalıştırmadan önce şu hale getirir:

echo "Merhaba kullanici, evin: /home/kullanici"
Sonra echo komutu bu metni ekrana basar.

Senin bu kodlarda yaptığın şey ne?
Sen kendi yazdığın mini shell’de aynı mekanizmayı implement ediyorsun. Yani:

Kullanıcı bir komut yazıyor:

echo Merhaba $USER
Sen bu stringi okuyorsun.

String içinde $ var mı diye bakıyorsun.
Eğer $ varsa, $'den sonra gelen ismin uzunluğunu buluyorsun (get_len_var ile).
Bu ismi alıp, envp dizisinde karşılığını buluyorsun.
Bulduğun değeri stringin içine yerleştiriyorsun (create_sub_var ile).
Özel bir durum: $? → bu global değişken g_exitstatus ile tutuluyor ve handle_g_exitstatus ile ekleniyor.

Neden "parçalamak" zorundayız?
Çünkü $ sadece değişkenin adını temsil ediyor, değerini değil, örneğin;

"echo $USER" → senin programın önce $USER'i USER değişkeninin değeri ile değiştirmeli.
Eğer değiştirmezsen, execve fonksiyonu $USER diye bir dosya arar, bulamaz → hata olur.
Yani expander fonksiyonları, komut çalışmadan önce $'li kısımları gerçek değerleriyle değiştirip, komutun doğru çalışmasını sağlıyor.

Basit örnek üzerinden işleyiş:
Diyelim env şu şekilde:

env[0] = "USER=ahmet";
env[1] = "HOME=/home/ahmet";
env[2] = NULL;

Kullanıcı şunu yazdı:
echo $HOME/Documents

Adımlar:
Program $ işaretini gördü → bunun ardından HOME kelimesi var.
HOME'un değerini env içinde buldu → /home/ahmet.
$HOME/Documents → /home/ahmet/Documents oldu.

Son komut:
echo /home/ahmet/Documents
olarak execve'ye gidecek.

*/



/*
  Expands environment variables in the array of strings `str` using
  the environment variables `ev`.
  It creates a new array with expanded strings and returns it.

  Parameters:
    - str: Array of strings to be expanded.
    - ev:  Array of strings representing environment variables.

  Returns:
    - A new array of strings with expanded environment variables.
*/

char	**expander(t_prompt *prompt, char **str, char **ev)
{
	int		i;
	char	**temp;

	temp = get_grbg(prompt, get_len_arr(str) + 1, sizeof(char *));
	if (!temp)
		return (NULL);
	i = 0;
	while (str[i])
	{
		temp[i] = expand_var(prompt, str[i], ev, 0);
		i++;
	}
	temp[i] = NULL;
	i = 0;
	while (temp[i] != NULL)
	{
		if (ft_strcmp(temp[i], "") == 0)
		{
			del_str(temp, i, 1);
			i = 0;
		}
		else
			i++;
	}
	return (temp);
}

/*
  Expands environment variables in the string `str` using
  the environment variables `ev`.
  It modifies the original string in-place and returns it.

  Parameters:
    - str: Pointer to the string to be expanded.
    - ev:  Array of strings representing environment variables.

  Returns:
    - Pointer to the modified string with expanded environment variables.
*/
char	*expand_var(t_prompt *prompt, char *str, char **ev, int i)
{
	int		q[4];
	char	*sub_str;

	(void)ev;
	sub_str = NULL;
	q[0] = 0;
	q[1] = 0;
	q[2] = i;
	q[3] = 0;
	if (ft_strcmp(str, "$") && ft_strlen(str) == 3)
		return (str);
	sub_str = handle_expansion(prompt, str, q, sub_str);
	if (sub_str)
		return (sub_str);
	else
		return (str);
}

char	*handle_g_exitstatus(t_prompt *prompt, int i, char *str, char *sub_str)
{
	char	*nb;
	int		len;

	len = 0;
	nb = 0;
	if (str[i + 1] == '?')
	{
		nb = grbg_itoa(prompt, g_exitstatus);
		if (!nb)
			return (NULL);
		len = ft_strlen(nb);
		sub_str = create_sub(str, i, nb, len);
		collect_grbg(prompt, sub_str);
	}
	return (sub_str);
}

/*
q[2] // i
q[3] //len
q[4] //
*/
char	*handle_expansion(t_prompt *prompt, char *str, int q[4], char *sub_str)
{
	while (str[q[2]])
	{
		q[0] = (q[0] + (!q[1] && str[q[2]] == '\'')) % 2;
		q[1] = (q[1] + (!q[0] && str[q[2]] == '\"')) % 2;
		if (!q[0] && str[q[2]] == '$' && str[q[2] + 1] && str[q[2] + 1] != ' ')
		{
			if (str[q[2] + 1] == '?')
				sub_str = handle_g_exitstatus(prompt, q[2], str, sub_str);
			else
			{
				q[3] = get_len_var(str, q[2] + 1);
				sub_str = create_sub_var(str, q[2], prompt->envp, q[3]);
				collect_grbg(prompt, sub_str);
				if (sub_str == NULL)
				{
					sub_str = "";
					break ;
				}
			}
			str = sub_str;
		}
		q[2]++;
	}
	return (sub_str);
}
