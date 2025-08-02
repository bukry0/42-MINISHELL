/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:17:37 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:17:37 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	is_only_space(const char *str) // gönderilen stringdeki karakterlerin tamamı space vb mi yoksa farklı herhangi bir karakter var mı bunun tespitini yapan fonksiyon
{
	while (*str != '\0') // gönderilen stringin son karakterine kadar tek tek ilerle
	{
		if (!isspace(*str)) // karakter space değilse 
			return (0); // 0 döndür
		str++;
	}
	return (1); // eğer gönderilen stringde ki bütün karakterler space ve benzeri karakterler ise 1 döndürür
}

void	lexer(t_prompt *prompt)
{
	prompt->input_string = readline("minishell$ ");
	collect_grbg(prompt, prompt->input_string);
	signals_non_interactive();
	add_history(prompt->input_string);
	if (prompt->input_string == NULL)
	{
		prompt->stop = 1;
		exit_ms(0, prompt);
		return ;
	}
	if (ft_strlen(prompt->input_string) <= 0
		|| is_only_space(prompt->input_string))
	{
		prompt->stop = 1;
		return ;
	}
	prompt->input_string = handle_spaces(prompt, prompt->input_string, 0, 0);
	prompt->commands = split_input(prompt->input_string, prompt);
	if (prompt->commands == NULL)
	{
		prompt->stop = 1;
	}
	if (prompt->stop != 1)
		prompt->commands = expander(prompt, prompt->commands, prompt->envp);
}
