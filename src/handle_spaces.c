/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_spaces.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:15:14 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:15:14 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
  Calculates and returns the special length of the string `str`,
  which represents the number of special characters that are not
  preceded or followed by a space.

  Parameters:
    - str: Pointer to the string to calculate the special length for.

  Returns:
    - The number of special characters that are not preceded or followed
	by a space in the string.
*/

static int	special_len(char *str, int count, size_t i)
{
	while (str[i] && str[i + 1])
	{
		if (str[i + 1] && ((str[i] == '>' && str[i + 1] == '>')
				|| (str[i] == '<' && str[i + 1] == '<')))
		{
			if (i > 0 && str[i - 1] != ' ')
				count++;
			if (str[i + 2] && str[i + 2] != ' ')
				count++;
			i += 1;
		}
		else if (str[i] == '>' || str[i] == '<' || str[i] == '|')
		{
			if (i > 0 && str[i - 1] != ' ')
				count++;
			if (str[i + 1] && str[i + 1] != ' ')
				count++;
			i++;
		}
		i++;
	}
	count++;
	return (count);
}

static char	*allocate_mem(t_prompt *prompt, char *str)
{
	char	*new_str;
	int		len_str;

	len_str = 0;
	new_str = NULL;
	len_str = ft_strlen(str) + special_len(str, 0, 0) + 1;
	new_str = (char *)get_grbg(prompt, len_str, sizeof(char));
	if (!new_str)
		return (NULL);
	return (new_str);
}

static void	add_spaces_double(char *str, char **new_str, int *j, size_t *i)
{
	if (str[(*i) + 2] != ' ' && (*i) + 2 < ft_strlen(str))
	{
		(*new_str)[(*j)++] = str[(*i)++];
		(*new_str)[(*j)++] = str[(*i)++];
		(*new_str)[(*j)++] = ' ';
	}
	else if ((*i) == 0 || str[(*i) - 1] != ' ')
	{
		(*new_str)[(*j)++] = ' ';
		(*new_str)[(*j)++] = str[(*i)++];
		(*new_str)[(*j)++] = str[(*i)++];
	}
	else
	{
		(*new_str)[(*j)++] = str[(*i)++];
		(*new_str)[(*j)++] = str[(*i)++];
	}
	return ;
}

static	void	add_spaces(char *str, char **new_str, int *j, size_t *i)
{
	if ((str[(*i)] == '>' && str[(*i) + 1] == '>')
		|| (str[(*i)] == '<' && str[(*i) + 1] == '<'))
	{
		add_spaces_double(str, &(*new_str), &(*j), &(*i));
	}
	else if ((str[(*i)] == '>' || str[(*i)] == '<'
			|| str[(*i)] == '|') && str[(*i) + 1] != ' ')
	{
		(*new_str)[(*j)++] = str[(*i)++];
		(*new_str)[(*j)++] = ' ';
	}
	else if (i != 0 && (str[(*i)] == '>' || str[(*i)] == '<'
			|| str[(*i)] == '|') && str[(*i) - 1] != ' ')
	{
		(*new_str)[(*j)++] = ' ';
		(*new_str)[(*j)++] = str[(*i)++];
	}
	else
		(*new_str)[(*j)++] = str[(*i)++];
	return ;
}

char	*handle_spaces(t_prompt *prompt, char *str, size_t i, int j)
{
	char	*new_str;
	int		sgq;
	int		dbq;

	sgq = 0;
	dbq = 0;
	new_str = allocate_mem(prompt, str);
	while (str[i])
	{
		sgq = (sgq + (!dbq && str[i] == '\'')) % 2;
		dbq = (dbq + (!sgq && str[i] == '\"')) % 2;
		if (!sgq && !dbq)
		{
			add_spaces(str, &new_str, &j, &i);
		}
		else
			new_str[j++] = str[i++];
	}
	new_str[j] = '\0';
	return (new_str);
}
