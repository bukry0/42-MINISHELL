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

/* custom getenv for the parsed environment variables in the prompt struct */
char	*get_envp(t_prompt *prompt, char *name) // verilen isimdeki environment değişkeninin değerini döndüren fonksiyon
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
		i++; // sonraki ortam değişkeni stringine ilerle
	}
}

int	go_home_dir(t_prompt *prompt)
{
	char	*home_dir;
	char	*cwd_before;
	char	*cwd_after;

	home_dir = get_envp(prompt, "HOME");
	if (!home_dir)
		return (print_err_msg_lng("cd", "not set", "HOME"));
	cwd_before = (char *)getcwd(NULL, 0);
	collect_grbg(prompt, cwd_before);
	modify_envp(prompt, "OLDPWD", cwd_before);
	chdir(home_dir);
	cwd_after = (char *)getcwd(NULL, 0);
	collect_grbg(prompt, cwd_after);
	modify_envp(prompt, "PWD", cwd_after);
	return (0);
}

int	go_back_dir(t_prompt *prompt)
{
	char	*old_dir;
	char	*cwd_before;
	char	*cwd_after;

	old_dir = get_envp(prompt, "OLDPWD");
	if (!old_dir)
		return (print_err_msg_lng("cd", "not set", "OLDPWD"));
	printf("%s\n", old_dir);
	cwd_before = (char *)getcwd(NULL, 0);
	collect_grbg(prompt, cwd_before);
	modify_envp(prompt, "OLDPWD", cwd_before);
	chdir(old_dir);
	cwd_after = (char *)getcwd(NULL, 0);
	collect_grbg(prompt, cwd_after);
	modify_envp(prompt, "PWD", cwd_after);
	return (0);
}

int	cstm_cd(t_cmddat *cmd_data)
{
	DIR		*dir_user;
	char	*cwd_before;
	char	*cwd_after;

	if (cmd_data->full_cmd[2])
		return (print_err_msg_lng("cd", "too many arguments", NULL));
	if (cmd_data->full_cmd)
	{
		if (!cmd_data->full_cmd[1]) // cd ~ eklenecek
			return (go_home_dir(cmd_data->prompt));
		else if (!ft_strcmp(cmd_data->full_cmd[1], "-"))
			return (go_back_dir(cmd_data->prompt));
	}
	cwd_before = (char *)getcwd(NULL, 0);
	collect_grbg(cmd_data->prompt, cwd_before);
	modify_envp(cmd_data->prompt, "OLDPWD", cwd_before);
	dir_user = opendir(cmd_data->full_cmd[1]);
	if (!dir_user || chdir(cmd_data->full_cmd[1]) == -1)
		return (print_err_msg_lng("cd", "No such file or directory",
				cmd_data->full_cmd[1]));
	closedir(dir_user);
	cwd_after = (char *)getcwd(NULL, 0);
	collect_grbg(cmd_data->prompt, cwd_after);
	modify_envp(cmd_data->prompt, "PWD", cwd_after);
	return (0);
}
