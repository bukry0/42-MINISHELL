/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_minishell.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcili <bcili@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 16:08:25 by bcili             #+#    #+#             */
/*   Updated: 2025/07/21 16:08:25 by bcili            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*free and exit*/
void	exit_ms(int g_exitstatus, t_prompt *prompt)
{
	free_all(prompt);
	exit(g_exitstatus);
}

void	free_all(t_prompt *prompt)
{
	free_grbg(prompt->grbg_lst);
	rl_clear_history();
	if (prompt)
		free(prompt);
}

void	free_node_list(t_node *head)
{
	t_node	*current;
	t_node	*temp;

	current = head;
	while (current)
	{
		temp = current;
		current = current->next;
		if (temp && temp->data)
		{
			if (temp->data->full_cmd)
				free(temp->data->full_cmd);
			if (temp->data->full_path)
				free(temp->data->full_path);
			free(temp->data);
		}
		free(temp);
	}
}
