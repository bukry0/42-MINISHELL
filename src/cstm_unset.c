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

size_t	get_len_env(const char *s)
{
	size_t	l;

	l = 0;
	if (!s)
		return (0);
	while (s[l] && s[l] != '=')
		l++;
	return (l);
}

int	cstm_unset(t_cmddat *cmd)
{
	int		i;
	int		j;
	char	**envs;

	i = 0;
	envs = cmd->prompt->envp;
	if (!envs || !cmd->full_cmd)
		return (1);
	while (envs[i])
	{
		j = 1;
		while (cmd->full_cmd[j] && ft_strcmp(cmd->full_cmd[j], "_"))
		{
			if (!ft_strncmp(envs[i], cmd->full_cmd[j], get_len_env(envs[i]))
				&& get_len_env(envs[i]) == ft_strlen(cmd->full_cmd[j]))
				del_str(envs, i, 1);
			j++;
		}
		i++;
	}
	return (0);
}
