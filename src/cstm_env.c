/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cstm_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 15:57:13 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 15:57:13 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
Env Nedir?
env = environment variables (ortam değişkenleri).
Bir program çalıştığında, işletim sistemi ona bir ortam tablosu (environment) aktarır.
Bu tablo, programın çalışmasını etkileyen anahtar-değer çiftlerinden oluşur. Örn;

env
Çıktı:
PATH=/usr/local/bin:/usr/bin:/bin
HOME=/home/user
SHELL=/bin/bash
PWD=/home/user

Shell’deki Kullanımı
Varsayılan olarak tüm environment değişkenlerini listeler.
Eğer env VAR=değer komut şeklinde kullanırsan, sadece o komut için geçici bir ortam yaratır. Örn;

env MYVAR=42 ./program
→ ./program çalışırken ortamında MYVAR=42 olur.
Ama env tek başına yazıldığında sadece mevcut ortamı listeler.

Minishell’deki env Builtin’i
Senin yazdığın minishell’de genelde şu şekilde olur:
t_prompt->envp gibi bir struct içinde environment değişkenlerini tutarsın.
cstm_env() fonksiyonu, bu listeyi outfile’a yazar.
Sadece = içeren değişkenler yazdırılır. (örn. PATH=..., HOME=...)
Yani declare -x VAR ile sadece tanımlanmış ama değeri yoksa → env yazmaz.

Çıkış Durumu
Başarıyla listelediyse → 0
Eğer bilinmeyen bir opsiyon girildiyse (örn. env -u) senin minishell’in muhtemelen “hata” verir veya görmezden gelir → 1 döner.

Redirect ile Kullanımı
Çıkışı dosyaya yönlendirebilirsin:
env > all_vars.txt

Örnekler

Basit kullanım
env
Çıktı:
PATH=/usr/local/bin:/usr/bin:/bin
PWD=/home/user
HOME=/home/user
SHELL=/bin/bash

Yeni değişkenle birlikte komut çalıştırma
env MYTEST=hello echo $MYTEST
Çıktı:
hello
(Not: MYTEST sadece echo için geçerliydi, kalıcı olmadı.)
echo $MYTEST
Çıktı: (boş satır)

Hatalı argüman (bash’teki davranış)
env -X
Çıktı:
env: invalid option -- 'X'
Try 'env --help' for more information.

Senin minishell’de bu kısmı basit yapabilirsin → “env: invalid option” diyerek çıkış kodunu 1 yapabilirsin.

Export edilmiş değişkenler
export NAME=ali
env | grep NAME
Çıktı:
NAME=ali
Ama sadece export NAME dersen (değer yok), env çıktısında görünmez.

Notlar
echo, pwd, env üçlüsü → en temel builtin’ler.
env özellikle PATH değişkenini görmek için çok kullanılır:
PATH → shell’in komutları nerede arayacağını belirler.
Gerçek bash’te env aynı zamanda yeni bir environment ile komut çalıştırma işlevine sahip → senin minishell’inde sadece listeleme kısmı yeterli.

*/

int	cstm_env(t_cmddat *cmd_data) // env komutunun işlevini manuel olarak yapacak fonksiyon
{
	int	i; // ortam değişkenlerini sistemden alığ tutan değişkenin içinde gezinmek için kullanılacak index

	i = -1; // başlangıç değeri -1 çünkü ++i olarak kullanılıyor
	if (cmd_data->prompt->envp == NULL) // eğer ortam değişkenlerini tutan değişken boşsa
		return (1); // 1 döndür ve çık
	while (cmd_data->prompt->envp[++i]) // envp[0] ile başlayıp bütün ortam değişkenlerini satır satır gezer
	{
		if (ft_strchr(cmd_data->prompt->envp[i], '=')) // bulunduğumuz değişkende = karakteri varsa 
		{
			ft_putstr_fd(cmd_data->prompt->envp[i], cmd_data->outfile); // bu stringi istenilen dosyaya ya da std output a yaz
			ft_putstr_fd("\n", cmd_data->outfile); // sonuna newline koy
		}
	}
	return (0); // bütün ortam değişkenleri hatasız yazıldıysa 0 döndür ve çık
}
