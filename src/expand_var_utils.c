/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_var_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:09:25 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:09:25 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
Gets the lenght of the var to expand, starting from after the$
i is the position of the string */
ssize_t	get_len_var(char *str, int i)
{
	ssize_t	count;

	count = 0;
	while (str[i] && !ft_isspace(str[i]) && str[i] != '\"' && str[i] != '\'' )
	{
		count++;
		i++;
	}
	return (count);
}

/*
Searches for the value of the environment variable
specified by the string `str` within the array of environment variables `env`.
It returns a pointer to the value of the environment variable.

Parameters:
- str:          Pointer to the string representing the environment variable.
- var_exp_len:  Length of the environment variable to search for.
- env:          Array of strings representing environment variables.

Returns:
- Pointer to the value of the environment variable if found.
- Pointer to an empty string if the environment variable is not found.
*/
static char	*get_ptr_var(char *str, size_t var_exp_len, char **env)
{
	while (*env)
	{
		if (!ft_strncmp(str, *env, var_exp_len)
			&& *(*env + var_exp_len) == '=')
			return (*env + var_exp_len + 1);
		env++;
	}
	return (NULL);
}

/*
Creates a substring by expanding the environment
variable specified by the substring in the string
`str`. It constructs and returns a new string with the
 expanded environment variable.

Parameters:
- str: Pointer to the string containing the environment
variable to be expanded.
- i:   Index of the starting character of the environment variable substring.
- ev:  Array of strings representing environment variables.
- len: Length of the environment variable substring to expand.

  Returns:
    - Pointer to the newly created string with the expanded environment variable.
*/
char	*create_sub_var(char *str, size_t i, char **ev, ssize_t len)
{
	char	*s1;
	char	*s2;
	char	*s3;
	char	*expanded_tmp;
	char	*expanded_str;

	expanded_tmp = NULL;
	s1 = ft_substr(str, 0, i);
	s3 = ft_substr(str, i + len + 1, ft_strlen(str) - i - len);
	s2 = ft_strdup(get_ptr_var(&str[i + 1], len, ev));
	if (s2 != NULL )
	{
		expanded_tmp = ft_strjoin(s1, s2);
		expanded_str = ft_strjoin(expanded_tmp, s3);
	}
	free(s1);
	free(s3);
	if (s2 == NULL)
		return (NULL);
	else
	{
		free(s2);
		free(expanded_tmp);
	}
	return (expanded_str);
}

char	*create_sub(char *str, size_t i, char *nb, ssize_t len)
{
	char	*s1;
	char	*s3;
	char	*expanded_tmp;
	char	*expanded_str;

	s1 = ft_substr(str, 0, i);
	s3 = ft_substr(str, i + len + 1, ft_strlen(str) - i - len);
	expanded_tmp = ft_strjoin(s1, nb);
	expanded_str = ft_strjoin(expanded_tmp, s3);
	free(expanded_tmp);
	free(s1);
	free(s3);
	return (expanded_str);
}
