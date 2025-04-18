/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmonika <mmonika@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 12:29:57 by mmonika           #+#    #+#             */
/*   Updated: 2025/04/13 16:43:28 by mmonika          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

int	is_valid_var(char *var)
{
	int	i;

	if (!var || !var[0])
		return (0);
	if (!ft_isalpha(var[0]) && var[0] != '_')
		return (0);
	i = 0;
	while (var[i] && var[i] != '=')
	{
		if (!ft_isalnum(var[i]) && var[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

int	is_builtin(char *args)
{
	if (!args)
		return (0);
	else if (!ft_strcmp(args, "echo"))
		return (1);
	else if (!ft_strcmp(args, "cd"))
		return (1);
	else if (!ft_strcmp(args, "pwd"))
		return (1);
	else if (!ft_strcmp(args, "export"))
		return (1);
	else if (!ft_strcmp(args, "unset"))
		return (1);
	else if (!ft_strcmp(args, "env"))
		return (1);
	return (0);
}

int	execute_custom_builtin(char **tokens, t_shell *minishell)
{
	if (!ft_strcmp(tokens[0], "echo"))
		minishell->exit_code = ft_echo(tokens);
	if (!ft_strcmp(tokens[0], "cd"))
		minishell->exit_code = ft_cd(minishell, tokens);
	if (!ft_strcmp(tokens[0], "pwd"))
		minishell->exit_code = ft_pwd();
	if (!ft_strcmp(tokens[0], "export"))
		minishell->exit_code = ft_export(minishell, tokens);
	if (!ft_strcmp(tokens[0], "unset"))
		minishell->exit_code = ft_unset(minishell, tokens);
	if (!ft_strcmp(tokens[0], "env"))
		minishell->exit_code = ft_env(minishell, 0);
	return (minishell->exit_code);
}

void	execute_builtins(char **tokens, t_shell *shell)
{
	if (is_builtin(tokens[0]))
		execute_custom_builtin(tokens, shell);
	else
	{
		if (ft_execvp(tokens[0], tokens, shell) == FAIL)
			shell->exit_code = 127;
	}
}
