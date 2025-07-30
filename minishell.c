/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 15:27:34 by bcili             #+#    #+#             */
/*   Updated: 2025/07/16 15:27:34 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		g_exitstatus = 0; // program boyunca çağırıldığı her file da kullanılacak olan global değişkenin tanımı yapılıyor

void	launch_minishell(t_prompt *prompt)
{
	while (1)
	{
		signals_interactive();
		prompt->stop = 0;
		lexer(prompt);
		if (prompt->stop == 0)
			parser(prompt, 0, 0);
            if (cstm_lstsize(prompt->cmd_list) > 1 && prompt->stop == 0)
			pipe_infile_outfile(prompt->cmd_list);
		if (!prompt->stop)
        execute_cmds(prompt);
		prompt->cmd_list = NULL;
	}
}

/* no garbage collection for below function as it does not leak
set outfile to write end of pipe
set infile to read end of pipe */
void	pipe_infile_outfile(t_node *cmd_lst)
{
    int	*pip;
    
	while (cmd_lst->next != NULL)
	{
        pip = ft_calloc(2, sizeof(int));
		if (!pip)
        return ;
		if (pipe(pip) == -1)
		{
            free(pip);
			return ;
		}
		if (cmd_lst->data->outfile == 1)
        cmd_lst->data->outfile = pip[1];
		else
        close(pip[1]);
		if (cmd_lst->next->data->infile == 0)
        cmd_lst->next->data->infile = pip[0];
		else
        close(pip[0]);
		cmd_lst = cmd_lst->next;
		free(pip);
	}
	return ;
}

int	main(int argc, char *argv[], char **envp) // env = environment, ortam değişkenlerini içeren bir string dizisi (HOME/bcili/..., PATH, etc.)
{
    t_prompt	*prompt; // shell in çalışması için gerekli tüm bilgileri içeren değişken declare ediliyor

    prompt = NULL; // allocation yapılmadan önce başlangıç değeri veriliyor
    (void)argv; // argüman kullanılmayacağı için unused variable uyarısını engeller
    if (argc == 1) // sadece ./minishell komutu varsa çalışır
    {
        prompt = ft_calloc(1, sizeof(t_prompt)); // prompt için dinamik bellek (heap)'te t_prompt büyüklüğünde yer ayrılıyor
        init_prompt_struct(prompt, envp); // yer ayrılan prompt içerisindeki değişkenler için shell döngüleri başlamadan önce başlangıç değerleri atanıyor
    }
    else // ./minishell komutu dışında herhangi bir başka argüman varsa kabul edilmiyor programdan çıkılııyor
    {
        printf("Error! This Program Does Not Accept Any Argument.\n"); // fazladan argüman için uyarı mesajı yazdırılıyor
        exit(0); // error koduyla programdan çıkılıyor
    }
    launch_minishell(prompt); // shell için gerekli döngülerin başlatıldığı ve bütün komutların handle edildiği asıl isi yapam fonksiyon çağırılıyor
    return (0); // program başarılı biterse ana fonksiyon 0 döndürüyor
}
