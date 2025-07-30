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

int	cstm_env(t_cmddat *cmd_data)
{
	int	i;

	i = -1;
	if (cmd_data->prompt->envp == NULL)
		return (1);
	while (cmd_data->prompt->envp[++i])
	{
		if (ft_strchr(cmd_data->prompt->envp[i], '='))
		{
			ft_putstr_fd(cmd_data->prompt->envp[i], cmd_data->outfile);
			ft_putstr_fd("\n", cmd_data->outfile);
		}
	}
	return (0);
}
