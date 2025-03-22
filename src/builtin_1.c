/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_1.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmonika <mmonika@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 12:58:45 by mmonika           #+#    #+#             */
/*   Updated: 2025/03/22 16:14:25 by mmonika          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

int	ft_echo(char **args)
{
	int	i;
	int	line;

	i = 1;
	line = 1;
	while (args[i] && args[i][0] == '-' && args[i][1] == 'n')
	{
		if (ft_strspn(args[i] + 1, "n") != ft_strlen(args[i] + 1))
			break ;
		line = 0;
		i++;
	}
	while (args[i])
	{
		ft_putstr_fd(args[i], 1);
		if (args[i + 1])
			ft_putstr_fd(" ", 1);
		i++;
	}
	if (line)
		ft_putstr_fd("\n", 1);
	return (SUCCESS);
}

int	ft_pwd(void)
{
	char	*directory;

	directory = getcwd (NULL, 0);
	if (!directory)
		return (perror("pwd error"), free(directory), FAIL);
	write(1, directory, ft_strlen(directory));
	write(1, "\n", 1);
	free (directory);
	return (SUCCESS);
}

int	ft_env(t_shell *minishell, int export_flag)
{
	t_dlist	*current;

	current = minishell->denv;
	while (current)
	{
		if (export_flag == 1)
			printf("declare -x %s\n", (char *)current->content);
		else
			printf("%s\n", (char *)current->content);
		current = current->next;
	}
	return (SUCCESS);
}

void	update_unset(t_shell *minishell, char *var)
{
	t_dlist	*current;
	size_t	len;
	char	*env_var;

	if (ft_strcmp(var, "_") == 0)
		return ;
	current = minishell->denv;
	len = ft_strlen(var);
	while (current)
	{
		env_var = (char *)current->content;
		if (ft_strncmp (env_var, var, len) == 0 && env_var[len] == '=')
		{
			if (current->prev)
				current->prev->next = current->next;
			if (current->next)
				current->next->prev = current->prev;
			if (current == minishell->denv)
				minishell->denv = current->next;
			free (current->content);
			free (current);
			return ;
		}
		current = current->next;
	}
}

int	ft_unset(t_shell *minishell, char **args)
{
	int	i;

	i = 1;
	while (args[i])
	{
		if (!is_valid_var(args[i]))
		{
			ft_putstr_fd("unset: ", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putstr_fd(" is invalid\n", STDERR_FILENO);
			minishell->exit_code = 1;
		}
		else
			update_unset(minishell, args[i]);
		i++;
	}
	return (SUCCESS);
}
