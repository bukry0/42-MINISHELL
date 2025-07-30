/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_redirections.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:14:22 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:14:22 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
  Determines the type of redirection for the given string `str`
  and returns the corresponding type value.

  Parameters:
    - str: Pointer to the string representing a command or redirection.

  Returns:
    - 1 if the redirection is an infile-redirection ('<').
    - 2 if the redirection is a here_doc-redirection ('<<').
    - 3 if the redirection is a write to outfile ('>').
    - 4 if the redirection is an append to outfile ('>>').
    - 0 if no redirection type is detected.
*/
int	get_type(char *str)
{
	int	type;
	int	i;
	int	q[2];

	i = -1;
	type = 0;
	q[0] = 0;
	q[1] = 0;
	while (str[++i])
	{
		q[0] = (q[0] + (!q[1] && str[i] == '\'')) % 2;
		q[1] = (q[1] + (!q[0] && str[i] == '\"')) % 2;
		if (!q[0] && !q[1])
		{
			if (ft_strlen(str) == 1 && str[i] == '<' )
				type = 1;
			if (str[i] == '<' && str[i + 1] && str[i + 1] == '<')
				type = 2;
			if (type == 0 && str[i] == '>')
				type = 3;
			if (type == 3 && str[i + 1] && str[i + 1] == '>')
				type = 4;
		}
	}
	return (type);
}

/*
  Handles redirections within the commands
  stored in the prompt structure `ptr`.
  It iterates through the command list
  and processes each command to identify and handle redirections.

  Parameters:
    - ptr: Pointer to the prompt structure containing the command list.

  Returns: None
*/
void	handle_redir(t_prompt *ptr, int type)
{
	int			i;
	t_node		*current_node;
	t_cmddat	*cmd_data;

	current_node = ptr->cmd_list;
	while (current_node != NULL)
	{
		cmd_data = current_node->data;
		if (cmd_data)
		{
			i = 0;
			while (cmd_data->full_cmd[i])
			{
				type = get_type(cmd_data->full_cmd[i]);
				if (type < 5 && type > 0)
				{
					open_fd_redir(ptr, cmd_data, i, type);
					cmd_data->full_cmd = del_str(cmd_data->full_cmd, i, 2);
					i -= 1;
				}
				i++;
			}
		}
		current_node = current_node->next;
	}
}

/*
if save_fd > 1 , it means that it's already open and we need to close it
*/

int	open_file(char **cmds, int i, int *save_fd, int io_flags[2] )
{
	if (*save_fd > 1)
	{
		if (close(*save_fd) == -1)
			printf("Error while attempting to close a file");
	}
	if (cmds[i + 1])
	{
		if (io_flags[1] != 0)
			*save_fd = open(cmds[i + 1], io_flags[0], io_flags[1]);
		else
			*save_fd = open(cmds[i + 1], io_flags[0]);
		if (*save_fd == -1)
		{
			g_exitstatus = 1;
			print_err_msg(cmds[i + 1], "No such file or directory");
			return (1);
		}
	}
	else
	{
		syntax_error(NULL, cmds[i + 1]);
	}
	return (0);
}

/*
Indicates that the file should be...
O_RDONLY: opened in read-only mode.
O_WRONLY: opened in write-only mode.
O_CREAT: created if it does not exist.
O_TRUNC: truncated (emptied) if it already exists.
O_APPEND: Indicates that data should be appended to the
 end of the file during writing.
00644: octal value used to specify the file permissions when creating it.
In this case, 00644 grants read and write permissions to
the file owner, and read permissions to other users.
*/

int	get_flags(int type, int file_access_type)
{
	if (file_access_type == 0)
	{
		if (type == 1 || type == 2)
			return (O_RDONLY);
		if (type == 3)
			return (O_WRONLY | O_CREAT | O_TRUNC);
		if (type == 4)
			return (O_WRONLY | O_CREAT | O_APPEND);
	}
	else if (file_access_type == 1)
	{
		if (type == 1 || type == 2)
			return (0);
		if (type == 3 || type == 4)
			return (0644);
	}
	return (0);
}

int	open_fd_redir(t_prompt *prompt, t_cmddat *cmd_struct, int i, int type)
{
	int	io_flags[2];

	io_flags[0] = get_flags(type, 0);
	io_flags[1] = get_flags(type, 1);
	get_rid_quotes(prompt);
	if (type == 1)
		cmd_struct->file_open = open_file(cmd_struct->full_cmd,
				i, &cmd_struct->infile, io_flags);
	else if (type == 2)
		launch_heredoc(prompt, cmd_struct, i);
	else if (type == 3)
		cmd_struct->file_open = open_file(cmd_struct->full_cmd,
				i, &cmd_struct->outfile, io_flags);
	else
		cmd_struct->file_open = open_file(cmd_struct->full_cmd,
				i, &cmd_struct->outfile, io_flags);
	return (0);
}
