/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:21:48 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:21:48 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
Iterate over the two-dimensional array
Whenever a redirection is found, check the type of
redirection and retrieve a file descriptor containing t
he info we need as the infile
Check that the file descriptor that has been opened is valid (!= -1)
and continue
If a pipe is found, add a new node to the list of commands
In all other cases add whatever words are found
 to the argument list (argv) we call full_cmd
*/

/*
  Creates and fills a new array of strings from the `prompt` array.
  It starts filling from index `i` and fills `len` number of elements.
  Memory is dynamically allocated for the new array.

  Parameters:
      main_prompt: Pointer to the prompt structure
    - prompt: Pointer to the original array of strings.
    - i:      Starting index to begin filling from.
    - len:    Number of elements to fill in the new array.

  Returns:
    - Pointer to the newly created array of strings.
*/
char	**fill_arr(t_prompt *main_prompt, char **prompt, int i, int len)
{
	char	**temp;
	int		j;

	j = 0;
	temp = NULL;
	temp = get_grbg(main_prompt, len + 1, sizeof(char *));
	if (!temp)
		return (NULL);
	while (len > 0)
	{
		temp[j] = grbg_strdup(main_prompt, prompt[i]);
		i++;
		j++;
		len--;
	}
	temp[j] = NULL;
	return (temp);
}

/*
Checks if the last character of
the command in the prompt is either '<', '>', or '|'.
If it's the case, prompt->stop = 1 and throw syntax error.
Parameters:
    - prompt: Pointer to the prompt structure containing input commands.

Returns:
- No return value.
*/
static void	check_last_char(t_prompt *prompt)
{
	int		len_ar;
	char	last_char;

	if (check_double_pipes(prompt) == 1)
	{
		prompt->stop = 1;
		return ;
	}
	len_ar = get_len_arr(prompt->commands) - 1;
	if (len_ar < 0 || prompt->commands[len_ar] == NULL
		|| prompt->commands[len_ar][0] == '\0')
		return ;
	last_char = prompt->commands[len_ar][0];
	if (last_char == '|' || last_char == '<' || last_char == '>' )
	{
		prompt->stop = 1;
		syntax_error(prompt, prompt->commands[len_ar]);
	}
}

/*
Checks if the last character of EACH command in the prompt is either '<', '>'.
If it's the case, prompt->stop = 1 and throw syntax error
Parameters:
    - prompt: Pointer to the prompt structure containing input commands.

Returns:
- No return value.
*/
static void	check_token(t_prompt *prompt)
{
	t_node		*current_node;
	t_cmddat	*cmd_data;
	int			len_arr;
	char		last_char;

	current_node = prompt->cmd_list;
	while (current_node != NULL)
	{
		cmd_data = current_node->data;
		len_arr = get_len_arr(cmd_data->full_cmd) - 1 ;
		last_char = cmd_data->full_cmd[len_arr][0];
		if (last_char == '|' || last_char == '<' || last_char == '>' )
		{
			prompt->stop = 1;
			syntax_error(prompt, cmd_data->full_cmd[len_arr]);
			free_node_list(prompt->cmd_list);
			prompt->cmd_list = NULL;
			break ;
		}
		current_node = current_node->next;
	}
}

/*might lead to double frees with gc,
but seems to work @DENIZ DOUBLE FREE if error
@deniz is this comment still accurate (22/04)*/
static void	parse_remaining(t_prompt *prompt)
{
	int	type;

	type = 0;
	check_token(prompt);
	if (prompt->stop == 0)
		handle_redir(prompt, type);
	if (prompt->cmd_list != NULL)
		add_last_cmd_to_envp(prompt);
	get_rid_quotes(prompt);
}

void	parser(t_prompt *prompt, int i, int j)
{
	t_cmddat	*ptr;

	ptr = NULL;
	check_last_char(prompt);
	while (prompt && prompt->commands && prompt->commands[i] != NULL
		&& prompt->stop == 0)
	{
		ptr = init_struct_cmd(prompt);
		if (!ptr)
			return ;
		add_node_to_list(prompt, &(prompt->cmd_list), ptr);
		while (prompt->commands[i] != NULL && prompt->commands[i][0] != '|')
		{
			i++;
			j++;
		}
		ptr->full_cmd = fill_arr(prompt, prompt->commands, i - j, j);
		ptr->full_path = get_path_cmds(ptr, prompt->envp);
		collect_grbg(prompt, ptr->full_path);
		if (prompt->commands[i] == NULL)
			break ;
		i++;
		j = 0;
	}
	parse_remaining(prompt);
}
