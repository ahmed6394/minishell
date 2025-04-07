/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gahmed <gahmed@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 15:00:37 by mmonika           #+#    #+#             */
/*   Updated: 2025/04/07 12:56:27 by gahmed           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../inc/minishell.h"

void	init_shell(t_shell *shell, char **env)
{
	shell->denv = convert_env_to_list(env);
	shell->env = env;
	shell->exit_code = 0;
	shell->old_pwd = NULL;
	shell->heredoc_fd = -1;
	shell->input_fd = 0;
	shell->is_piped = 0;
}

void	process_input(t_shell *shell, char *input)
{
	char	**tokens;
	char	**commands;
	char	*final_input;

	final_input = expand_variables(input, shell);
	if (!final_input || !*final_input)
		return ;
	if (ft_strchr(final_input, '|'))
	{
		commands = split_pipes(final_input);
		if (commands)
		{
			execute_piped_commands(commands, shell);
			ft_free_tab(commands);
		}
	}
	else
	{
		tokens = tokenize_input(final_input, shell);
		if (tokens && tokens[0])
			execute_single_commands(tokens, shell);
		ft_free_tab(tokens);
	}
	free(final_input);
}

int	main(int ac, char **av, char **env)
{
	t_shell	shell;
	char	*input;

	(void)av;
	if (ac != 1)
		return (printf("Invalid Input\n"), 0);
	signal_handler();
	init_shell(&shell, env);
	while (1)
	{
		input = readline("\033[33mminishell$ \033[0m");
		if (!input || ft_strncmp(input, "exit", 4) == 0)
			ft_exit(&shell, input);
		if (*input)
			add_history(input);
		if (check_input(&shell, input) == 0)
			process_input(&shell, input);
		free(input);
	}
	return (shell.exit_code);
}
