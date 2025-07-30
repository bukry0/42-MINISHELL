/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:23:47 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:23:47 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
  Removes unnecessary quotes from the commands stored in the prompt.
  It iterates over each command, trims unnecessary quotes from its arguments,
  and updates the prompt's command data accordingly.

  
  Parameters:
    - prompt: Pointer to the prompt containing the commands to be processed.

  Returns:
    - None.
*/
void	get_rid_quotes(t_prompt	*prompt)
{
	char		**temp;
	char		*trim_cmd;
	int			i[2];
	t_node		*current_node;
	t_cmddat	*cmd_data;

	current_node = prompt->cmd_list;
	while (current_node != NULL)
	{
		cmd_data = current_node->data;
		i[0] = 0;
		i[1] = get_len_arr(cmd_data->full_cmd);
		temp = get_grbg(prompt, i[1] + 1, sizeof(char *));
		if (!temp)
			return ;
		while (cmd_data->full_cmd[i[0]])
		{
			trim_cmd = get_trimmed(prompt, cmd_data->full_cmd[i[0]], 0, 0);
			temp[i[0]] = trim_cmd;
			i[0]++;
		}
		cmd_data->full_cmd = temp;
		current_node = current_node->next;
	}
}

/*
  Removes unnecessary quotes from a str while preserving quoted substrings.
  It returns a new dynamically allocated string containing the trimmed result.

  Parameters:
    - s1: Pointer to the input string to be trimmed.
    - squote: Integer representing the state of single quotes in the string.
    - dquote: Integer representing the state of double quotes in the string.

  Returns:
    - Pointer to the dynamically allocated trimmed string.
*/
char	*get_trimmed(t_prompt *prompt, char const *s1, int squote, int dquote)
{
	int		count;
	int		i[2];
	char	*trimmed;

	i[1] = -1;
	i[0] = 0;
	count = malloc_len(s1);
	if (!s1 || count == -1)
		return (NULL);
	trimmed = get_grbg(prompt, ft_strlen(s1) - count + 1, sizeof(char));
	if (!trimmed)
		return (NULL);
	while (s1[i[0]])
	{
		squote = (squote + (!dquote && s1[i[0]] == '\'')) % 2;
		dquote = (dquote + (!squote && s1[i[0]] == '\"')) % 2;
		if ((s1[i[0]] != '\"' || squote) && (s1[i[0]] != '\'' || dquote) \
			&& ++i[1] >= 0)
			trimmed[i[1]] = s1[i[0]];
		i[0]++;
	}
	trimmed[++i[1]] = '\0';
	return (trimmed);
}

/*
  Counts the number of unnecessary quotes in the string `str`.
  It checks the consistency of quotes and returns the count.

  Parameters:
    - str: Pointer to the input string to be checked.

  Returns:
    - Number of unnecessary quotes in the string.
*/
int	malloc_len(char const *str)
{
	int	count;
	int	squote;
	int	dquote;
	int	i;

	count = 0;
	squote = 0;
	dquote = 0;
	i = 0;
	while (str[i])
	{
		squote = (squote + (!dquote && str[i] == '\'')) % 2;
		dquote = (dquote + (!squote && str[i] == '\"')) % 2;
		if ((str[i] == '\"' && !squote) || (str[i] == '\'' && !dquote))
			count++;
		i++;
	}
	if (squote || dquote)
		return (-1);
	return (count);
}
