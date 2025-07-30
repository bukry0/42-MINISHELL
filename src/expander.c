/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:10:19 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:10:19 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
  Expands environment variables in the array of strings `str` using
  the environment variables `ev`.
  It creates a new array with expanded strings and returns it.

  Parameters:
    - str: Array of strings to be expanded.
    - ev:  Array of strings representing environment variables.

  Returns:
    - A new array of strings with expanded environment variables.
*/

char	**expander(t_prompt *prompt, char **str, char **ev)
{
	int		i;
	char	**temp;

	temp = get_grbg(prompt, get_len_arr(str) + 1, sizeof(char *));
	if (!temp)
		return (NULL);
	i = 0;
	while (str[i])
	{
		temp[i] = expand_var(prompt, str[i], ev, 0);
		i++;
	}
	temp[i] = NULL;
	i = 0;
	while (temp[i] != NULL)
	{
		if (ft_strcmp(temp[i], "") == 0)
		{
			del_str(temp, i, 1);
			i = 0;
		}
		else
			i++;
	}
	return (temp);
}

/*
  Expands environment variables in the string `str` using
  the environment variables `ev`.
  It modifies the original string in-place and returns it.

  Parameters:
    - str: Pointer to the string to be expanded.
    - ev:  Array of strings representing environment variables.

  Returns:
    - Pointer to the modified string with expanded environment variables.
*/
char	*expand_var(t_prompt *prompt, char *str, char **ev, int i)
{
	int		q[4];
	char	*sub_str;

	(void)ev;
	sub_str = NULL;
	q[0] = 0;
	q[1] = 0;
	q[2] = i;
	q[3] = 0;
	if (ft_strcmp(str, "$") && ft_strlen(str) == 3)
		return (str);
	sub_str = handle_expansion(prompt, str, q, sub_str);
	if (sub_str)
		return (sub_str);
	else
		return (str);
}

char	*handle_g_exitstatus(t_prompt *prompt, int i, char *str, char *sub_str)
{
	char	*nb;
	int		len;

	len = 0;
	nb = 0;
	if (str[i + 1] == '?')
	{
		nb = grbg_itoa(prompt, g_exitstatus);
		if (!nb)
			return (NULL);
		len = ft_strlen(nb);
		sub_str = create_sub(str, i, nb, len);
		collect_grbg(prompt, sub_str);
	}
	return (sub_str);
}

/*
q[2] // i
q[3] //len
q[4] //
*/
char	*handle_expansion(t_prompt *prompt, char *str, int q[4], char *sub_str)
{
	while (str[q[2]])
	{
		q[0] = (q[0] + (!q[1] && str[q[2]] == '\'')) % 2;
		q[1] = (q[1] + (!q[0] && str[q[2]] == '\"')) % 2;
		if (!q[0] && str[q[2]] == '$' && str[q[2] + 1] && str[q[2] + 1] != ' ')
		{
			if (str[q[2] + 1] == '?')
				sub_str = handle_g_exitstatus(prompt, q[2], str, sub_str);
			else
			{
				q[3] = get_len_var(str, q[2] + 1);
				sub_str = create_sub_var(str, q[2], prompt->envp, q[3]);
				collect_grbg(prompt, sub_str);
				if (sub_str == NULL)
				{
					sub_str = "";
					break ;
				}
			}
			str = sub_str;
		}
		q[2]++;
	}
	return (sub_str);
}
