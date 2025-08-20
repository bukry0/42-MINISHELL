/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 15:52:58 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 15:52:58 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	get_builtin_nbr(t_cmddat *cmd) // girilen kodun karakterlerini kontrol eder ve hangi kodun builltin olup olmadığına göre true veya false değerler döndürür ve eğer komut builtins ise hangi builtins olduğunu farklı rakamlar göndererek belirtir örneğin; echo:1, cd:2, pwd:3, export:4, unset:5, env:6, exit:7 döndüren fonksiyon
{
	if (cmd->full_cmd == NULL || cmd->full_cmd[0] == NULL) // eğer gönderilen komut içeriği boşsa
		return (0); // 0 döndür ve çık
	else if (!ft_strcmp(cmd->full_cmd[0], "echo")) // eğer gönderilen komut echo ise
		return (1); // 1 döndür ve çık echo: 1
	else if (!ft_strcmp(cmd->full_cmd[0], "cd")) // eğer gönderilen komut cd ise
		return (2); // 2 döndür ve çık cd: 2
	if (!ft_strcmp(cmd->full_cmd[0], "pwd")) // eğer gönderilen komut pwd ise
		return (3); // 3 döndür ve çık pwd:3
	else if (!ft_strcmp(cmd->full_cmd[0], "export")) // eğer gönderilen komut export ise
		return (4); // 4 döndür ve çık
	else if (!ft_strcmp(cmd->full_cmd[0], "unset")) // eğer gönderilen komut unset ise
		return (5); // 5 döndür ve çık
	else if (!ft_strcmp(cmd->full_cmd[0], "env")) // eğer gönderilen komut env ise
		return (6); // 6 döndür ve çık
	else if (!ft_strcmp(cmd->full_cmd[0], "exit")) // eğer gönderilen komut exit ise
		return (7); // 7 döndür ve çık
	return (0); // eğer komut boş değil ama builtin olarak ayrılan komutlardan biride değilse 0 döndür ve çık
}

/* g_exitstatus 127 when command to execute could not be found */
int	execute_builtin(t_cmddat *cmd, int n, int forked) // hangi builtin komutu olduğu numaralandırılarak n ile gönderilen komutu istenilen şekilde çalıştıracak fonksiyona gönderen fonksiyon
{
	int	r;

	if (n == 1) // eğer tespit edilen builtin komutu değeri 1: echo ise
		r = cstm_echo(cmd); // echo komutunu çalıştıran fonksiyonu çağır
	else if (n == 2) // eğer tespit edilen builtin komutu değeri 2: cd ise
		r = cstm_cd(cmd); // cd komutunu çalıştıran fonksiyonu çağır
	else if (n == 3) // eğer tespit edilen builtin komutu değeri 3: pwd ise
		r = cstm_pwd(cmd); // pwd komutunu çalıştıran fonksiyonu çağır
	else if (n == 4) // eğer tespit edilen builtin komutu değeri 4: export ise
		r = cstm_export(cmd); // export komutunu çalıştıran fonksiyonu çağır
	else if (n == 5) // eğer tespit edilen builtin komutu değeri 5: unset ise
		r = cstm_unset(cmd); // unset komutunu çalıştıran fonksiyonu çağır
	else if (n == 6) // eğer tespit edilen builtin komutu değeri 6: env ise
		r = cstm_env(cmd); // env komutunu çalıştıran fonksiyonu çağır
	else if (n == 7) // eğer tespit edilen builtin komutu değeri 7: exit ise
		r = cstm_exit(cmd); // exit komutunu çalıştıran fonksiyonu çağır
	else // eğer n değeri hiçbir builtin fonksiyonu değeri ile uyuşmuyorsa 
		r = 127; // exit kodunu 127 (shell de komut bulunamadı anlamına gelen kod) zaten bu fonksiyonun return değeri g_exitstatus olarak kullanılıyor
	if (forked) // bu builtin komutunu fork içinde çağırdıysak (1: child process içindeyiz 0: parent (main shell) process içindeyiz)
	{
		cstm_lstiter(cmd->prompt->cmd_list, cls_fds); // dosya tanıtıcılarını kapatılır çünkü pipe/redirect varsa, child işini bitirdiğinde bunlar açık kalmamalı
		exit_ms(r, cmd->prompt); // exit yapıyor yani child process burada bitiyor r ile de g_exitstatus döndürülüyor
	}
	return (r); // eğer builtin ana shell process içinde çalışıyorsa (forked == 0), child çıkışı yapılmaz sadece r döner yani shell açık kalır, çalışmaya devam eder
}
