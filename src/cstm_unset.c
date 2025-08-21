/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cstm_unset.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:01:15 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:01:15 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
Unset Nedir?
unset → ortam değişkenlerini (environment variables) silmek için kullanılan builtin komut.
Shell’de environment (çevre değişkenleri), programların çalışırken kullandığı key=value çiftleridir. Örneğin:
export PATH="/usr/bin:/bin"
export USER="ahmet"
Burada PATH ve USER environment variable’dır.
unset kullanarak bir değişkeni tamamen silebilirsin.

unset USER
→ Artık $USER yoktur.

Kullanımı
unset [değişken_adı ...]
Argüman olarak verilen değişken(ler) environment’tan kaldırılır.
Eğer birden fazla argüman verilirse hepsi sırayla silinir.
Dikkat: unset değişkenin değerini boş yapmaz → değişkeni komple siler.

Örnekler
Basit unset
$ export TEST=hello
$ echo $TEST
hello
$ unset TEST
$ echo $TEST

(echo boş döner → değişken silindi.)

Birden fazla değişken silme
$ export A=1
$ export B=2
$ export C=3
$ unset A B
$ echo $A $B $C
3

Sadece C kaldı.

Var olmayan değişkeni unset
$ unset NOT_FOUND
→ Hiçbir şey yapmaz, hata vermez.

Özel değişken _
Bash’te _ (underscore) özel bir değişkendir.
Genellikle son çalıştırılan komutun yolu gibi şeyler tutulur.
unset _ yapsan da bash yeniden atar.
Senin fonksiyonunda da _ değişkeni özellikle silinmemesi için kontrol edilmiş.

Script içinde unset
#!/bin/bash
export SECRET="1234"
unset SECRET
echo $SECRET
Çalıştırınca → hiçbir şey yazmaz çünkü SECRET artık yok.

Notlar
unset sadece shell’in kendi environment’ını etkiler.
Çocuğa (child process) fork yapıldığında, unset edilmiş değişken o process’e aktarılmaz.
Bu yüzden unset PATH yaparsan:

$ unset PATH
$ ls
bash: ls: No such file or directory
Çünkü ls için PATH’te arama yapamazsın.

*/

size_t	get_len_env(const char *s) // key-value olarak kaydedilen env (ortam değişkenlerinin) key değerinin kaç karakter olduğunu sayan fonksiyon
{
	size_t	l; // uzunluğu tutacak değişken

	l = 0; // başlangıçta 0
	if (!s) // eğer içinde env değeri olması gereken string boşsa
		return (0); // uzunluğu 0 döndür ve çık
	while (s[l] && s[l] != '=') // string boş değilse ve elimdeki karakter = değilse (= key ile value değeri arasında bulunduğu için biz onu seperator olarak kullanırız)
		l++; // uzunluğu 1 arttır
	return (l); // elde ettiğin key değeri uzunluğunu döndür (örn: USER=bcili -> l=4)
}

int	cstm_unset(t_cmddat *cmd) // unset (env değişkeni silme) komutunun işlevini manuel olarak yapan fonksiyon
{
	int		i; // env dizisi elemanları içinde dolaşmamızı sağlayacak index
	int		j; // komut satırındaki argümanları gezmemizi sağlayacak değişken
	char	**envs; // ortam değişkenlerini geçici olarak tutacak değişken

	i = 0; // ilk env elemanından başlar
	envs = cmd->prompt->envp; // ortam değişkenlerini geçici değişkene kopyala
	if (!envs || !cmd->full_cmd) // eğer ortam değişkeni boşsa ya da komut satırı boşsa
		return (1); // 1 döndür ve çık
	while (envs[i]) // bütün ortam değişkeni key-value çiftlerini gez
	{
		j = 1; // unset komutundan sonraki komut olan 2. argümandan başla
		while (cmd->full_cmd[j] && ft_strcmp(cmd->full_cmd[j], "_")) // komut satırında unsetten sonraki argüman boş değilse ve _ (underscore) değilse (çünkü önceki komutun yolunu tutar history yi kaybetmemek için)
		{
			if (!ft_strncmp(envs[i], cmd->full_cmd[j], get_len_env(envs[i]))
				&& get_len_env(envs[i]) == ft_strlen(cmd->full_cmd[j])) // komut satırındaki 2. argüman ile içerisinde gezdiğimiz env değişkeninin key değeri aynıysa (unset USER yazdın ve envs[i] = "USER=bcili")
				del_str(envs, i, 1); // istenilen ortam değişkeni bulunduğu için sil
			j++; // sonraki argümana geç çünkü tek satırda birden fazla ortam değişkeni silinebilir
		}
		i++; // sonraki ortam değişkenine geç
	}
	return (0); // fonksiyon başarılı biterse 0 döndür ve çık
}
