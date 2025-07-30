/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:15:58 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:15:58 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	launch_heredoc(t_prompt *prompt, t_cmddat *cmd, int i)
{
	char	*lim;
	size_t	j;

	lim = cmd->full_cmd[i + 1];
	j = 0;
	while (ft_isalnum(lim[j]))
		j++;
	if (j != ft_strlen(lim))
	{
		ft_putstr_fd("minishell: input error: delimiter must ", 2);
		ft_putstr_fd("contain only alphanumeric characters\n", 2);
		prompt->stop = 1;
		return ;
	}
	cmd->infile = get_heredoc(prompt, lim);
	if (g_exitstatus == 1)
		prompt->stop = 1;
}

int	get_heredoc(t_prompt *prompt, char *lim)
{
	char	*content;
	char	*line;

	content = NULL;
	line = NULL;
	g_exitstatus = 0;
	while (1)
	{
		signals_interactive();
		line = readline("> ");
		collect_grbg(prompt, line);
		signals_non_interactive();
		if (!line)
		{
			print_err_msg("warning", "here-document delimited by end-of-file");
			break ;
		}
		if (!ft_strncmp(line, lim, ft_strlen(line))
			&& ft_strlen(line) == ft_strlen(lim))
			break ;
		line = expand_var(prompt, line, prompt->envp, 0);
		content = add_to_str(prompt, &content, line);
		content = add_to_str(prompt, &content, "\n");
	}
	return (pipe_heredoc(content));
}

int	pipe_heredoc(char *content)
{
	int	pip[2];

	if (g_exitstatus)
		return (0);
	if (!pipe(pip))
	{
		ft_putstr_fd(content, pip[1]);
		close(pip[1]);
		return (pip[0]);
	}
	return (0);
}
