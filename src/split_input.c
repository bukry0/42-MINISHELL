/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:26:38 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:26:38 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
Counts the number of words in the given string `str`,
considering a set of separators defined by `sep`.
It handles cases where words might be enclosed within single or double quotes,
ensuring they are not counted separately.
If there's a mismatch in quotes, it returns -1 to indicate an error.
*/
static int	ft_count_words_ms(const char *str, char *sep, int count, int i)
{
	int	q[2];

	q[0] = 0;
	q[1] = 0;
	while (str[i] != '\0')
	{
		if (!ft_strchr(sep, str[i]))
		{
			count++;
			while ((!ft_strchr(sep, str[i]) || q[0]) && str[i] != '\0')
			{
				if (!q[1] && (str[i] == '\"' || str[i] == '\''))
					q[1] = str[i];
				if (str[i] == q[1])
					q[0] = (q[0] + 1) % 2;
				q[1] = q[1] * (q[0] != 0);
				i++;
			}
			if (q[0])
				return (-1);
		}
		else
			i++;
	}
	return (count);
}

/*
	i[0]
	i[1] : start_index
	i[2] : word_index
*/
char	**ft_create_substrs(t_prompt *p, char **aux, char const *s, char *set)
{
	int		i[3];
	int		q[2];

	q[1] = 0;
	q[0] = 0;
	i[0] = 0;
	i[1] = 0;
	i[2] = 0;
	while (s[i[0]])
	{
		while (ft_strchr(set, s[i[0]]) && s[i[0]] != '\0')
			i[0]++;
		i[1] = i[0];
		while ((!ft_strchr(set, s[i[0]]) || q[0] || q[1]) && s[i[0]])
		{
			q[0] = (q[0] + (!q[1] && s[i[0]] == '\'')) % 2;
			q[1] = (q[1] + (!q[0] && s[i[0]] == '\"')) % 2;
			i[0]++;
		}
		if (i[1] >= (int)ft_strlen(s))
			aux[i[2]++] = NULL;
		else
			aux[i[2]++] = grbg_substr(p, s, i[1], i[0] - i[1]);
	}
	return (aux);
}

/* Split the input read from the readline and put it into
	char		**commands; from the original prompt struct
*/
char	**split_input(char *str, t_prompt *prompt)
{
	char	**arr;
	int		word_count;

	if (!str)
		return (NULL);
	if (ft_isspace(*str))
		str++;
	word_count = ft_count_words_ms(str, " ", 0, 0);
	if (word_count == -1)
	{
		printf("Syntax error: unclosed quote in argument\n");
		return (NULL);
	}
	arr = get_grbg(prompt, word_count + 1, sizeof(char *));
	if (!arr)
		return (NULL);
	arr = ft_create_substrs(prompt, arr, str, " ");
	arr[word_count] = NULL;
	return (arr);
}
