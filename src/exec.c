/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:02:52 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:02:52 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_executable(t_cmddat *cmd_data)
{
	int	builtin;

	builtin = get_builtin_nbr(cmd_data);
	if (cmd_data->file_open > 0)
	{
		g_exitstatus = cmd_data->file_open;
		return (0);
	}
	if (!builtin && !cmd_data->full_path)
	{
		if (cmd_data->full_cmd[0])
			print_err_msg_lng(cmd_data->full_cmd[0], "command not found", NULL);
		g_exitstatus = 127;
		return (0);
	}
	else if (!builtin && cmd_data->full_path && (!access(cmd_data->full_path,
				F_OK) && access(cmd_data->full_path, X_OK) == -1))
	{
		if (cmd_data->full_cmd[0])
			print_err_msg_lng(cmd_data->full_cmd[0], "permission denied", NULL);
		g_exitstatus = 126;
		return (0);
	}
	return (1);
}

/*	Wait for first child process to terminate;
	if it exits normally (not by a signal),
	update temp_exitcode with the exit status of the child process.
	Wait for all child processes to terminate.
	If exitstatus changed while waiting, update existatus.
	F_OK checks that file exists, X_OK checks that file executable.	*/
void	wait_update_exitstatus(t_prompt *prompt)
{
	int			tmp_exitstatus;
	int			child_status;
	t_cmddat	*last;

	tmp_exitstatus = -1;
	last = cstm_lstlast(prompt->cmd_list)->data;
	if (waitpid(prompt->pid, &child_status, 0) != -1 && WIFEXITED(child_status))
		tmp_exitstatus = WEXITSTATUS(child_status);
	while (waitpid(-1, &child_status, 0) != -1)
		continue ;
	if (tmp_exitstatus != -1)
		g_exitstatus = tmp_exitstatus;
	if (last->file_open != 0)
		g_exitstatus = last->file_open;
	else if (!last->full_path && !get_builtin_nbr(last))
		g_exitstatus = 127;
	else if (last->full_path && !get_builtin_nbr(last)
		&& (!access(last->full_path, F_OK) && access(last->full_path,
				X_OK < 0)))
		g_exitstatus = 126;
	return ;
}

void	cls_fds(void *content)
{
	t_cmddat	*cmd_data;

	cmd_data = (t_cmddat *)content;
	if (cmd_data->infile != -1 && cmd_data->infile != 0)
		close(cmd_data->infile);
	if (cmd_data->outfile != -1 && cmd_data->outfile != 1)
		close(cmd_data->outfile);
}

/* exitstatus is 2 when misuse of shell builtins -> e.g.,
	ls: cannot access command-line argument */
void	run_cmd(void *content)
{
	t_cmddat	*cmd_data;

	cmd_data = (t_cmddat *)content;
	if (cmd_data->prompt->stop == 1)
		return ;
	if (!is_executable(cmd_data))
		return ;
	cmd_data->prompt->pid = fork();
	if (cmd_data->prompt->pid == -1)
		return ;
	if (cmd_data->prompt->pid == 0)
	{
		if (get_builtin_nbr(cmd_data))
			execute_builtin(cmd_data, get_builtin_nbr(cmd_data), 1);
		dup2(cmd_data->infile, 0);
		dup2(cmd_data->outfile, 1);
		cstm_lstiter(cmd_data->prompt->cmd_list, cls_fds);
		execve(cmd_data->full_path, cmd_data->full_cmd, cmd_data->prompt->envp);
		close(0);
		close(1);
		exit_ms(2, cmd_data->prompt);
	}
	return ;
}

int	execute_cmds(t_prompt *prompt)
{
	t_cmddat	*cmd_data;

	if (!prompt->cmd_list || !prompt->cmd_list->data->full_cmd[0])
		return (0);
	cmd_data = prompt->cmd_list->data;
	if (cstm_lstsize(prompt->cmd_list) == 1 && get_builtin_nbr(cmd_data))
	{
		g_exitstatus = execute_builtin(cmd_data, get_builtin_nbr(cmd_data), 0);
		cstm_lstiter(prompt->cmd_list, cls_fds);
	}
	else
	{
		cstm_lstiter(prompt->cmd_list, run_cmd);
		cstm_lstiter(prompt->cmd_list, cls_fds);
		wait_update_exitstatus(prompt);
	}
	return (0);
}
