/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_lib.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:11:57 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:11:57 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* custom libft functions with get_grbg instead of malloc or ft_calloc*/

char	*grbg_strdup(t_prompt *prompt, const char *s)
{
	size_t	len;
	size_t	i;
	char	*dest;

	i = 0;
	len = ft_strlen(s) + 1;
	dest = get_grbg(prompt, len, sizeof(char));
	if (dest == NULL)
		return (NULL);
	while (s[i] != '\0')
	{
		dest[i] = s[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

char	*grbg_substr(t_prompt *prompt, char const *s, unsigned int start,
		size_t len)
{
	size_t	i;
	char	*dest;

	i = 0;
	if (ft_strlen(s) < start)
		return (grbg_strdup(prompt, ""));
	if (ft_strlen(s + start) < len)
		len = ft_strlen(s + start);
	dest = get_grbg(prompt, len + 1, sizeof(char));
	if (!dest)
		return (NULL);
	while (i < len)
	{
		dest[i] = s[start + i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

char	*grbg_strjoin(t_prompt *prompt, char const *s1, char const *s2)
{
	size_t	len;
	char	*dest;

	len = ft_strlen(s1) + ft_strlen(s2) + 1;
	dest = get_grbg(prompt, sizeof(char), len);
	if (!dest)
		return (NULL);
	ft_strlcpy(dest, s1, len); // len = ft_strlen(s1) + 1
	ft_strlcat(dest, s2, len);
	return (dest);
}
