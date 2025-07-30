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
char	*get_envp(t_prompt *prompt, char *name)
{
	int		i;
	int		l;
	char	*str;
	char	*env_var;

	i = 0;
	if (!name)
		return (NULL);
	str = grbg_strjoin(prompt, name, "=");
	if (!str)
		return (NULL);
	l = ft_strlen(str);
	while (prompt->envp[i])
	{
		if (!ft_strncmp(prompt->envp[i], str, l))
		{
			env_var = grbg_strdup(prompt, prompt->envp[i] + l);
			return (env_var);
		}
		i++;
	}
	return (NULL);
}

void	modify_envp(t_prompt *prompt, char *name, char *insert)
{
	int		i;
	char	*str;

	i = 0;
	if (!prompt->envp || !insert || !name)
		return ;
	str = grbg_strjoin(prompt, name, "=");
	if (!str)
		return ;
	while (prompt->envp[i])
	{
		if (!ft_strncmp(prompt->envp[i], str, ft_strlen(str)))
			prompt->envp[i] = grbg_strjoin(prompt, str, insert);
		i++;
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
