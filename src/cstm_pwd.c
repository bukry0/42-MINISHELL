/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cstm_pwd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:00:20 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:00:20 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	cstm_pwd(t_cmddat *cmd_data)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	collect_grbg(cmd_data->prompt, cwd);
	if (!cwd)
		return (1);
	ft_putstr_fd(cwd, cmd_data->outfile);
	ft_putstr_fd("\n", cmd_data->outfile);
	return (0);
}
