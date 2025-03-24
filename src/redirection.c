/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmonika <mmonika@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 12:45:20 by gahmed            #+#    #+#             */
/*   Updated: 2025/03/24 15:14:15 by mmonika          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

int	handle_input_redirection(char **tokens, int *i)
{
	int	fd;

	if (!tokens[*i + 1])
	{
		ft_putstr_fd("missing file for input redirection\n", STDERR_FILENO);
		return (-1);
	}
	fd = open(tokens[*i + 1], O_RDONLY);
	if (fd < 0)
		return (perror("minishell: input redirection failed"), -1);
	if (dup2(fd, STDIN_FILENO) < 0)
	{
		perror("minishell: dup2 failed");
		close(fd);
		return (-1);
	}
	close(fd);
	*i += 1;
	return (0);
}

int	handle_output_redirection(char **tokens, int *i, int append)
{
	int	flags;
	int	fd;

	if (!tokens[*i + 1])
		return (ft_putstr_fd("minishell: syntax error\n", STDERR_FILENO), -1);
	flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC); //ternari
	fd = open(tokens[*i + 1], flags, 0644);
	if (fd < 0)
		return (perror("minishell:redirection failed"), -1);
	if (dup2(fd, STDOUT_FILENO) < 0)
	{
		perror("minishell: dup2 failed");
		close(fd);
		return (-1);
	}
	close(fd);
	*i += 1;
	return (0);
}

int	handle_heredoc(char *delimiter)
{
	char	*line;
	int		fd;

	fd = open("/tmp/minishell_heredoc", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return (perror("minishell: heredoc file creation failed"), -1);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
			break ;
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	free(line);
	close(fd);
	fd = open("/tmp/minishell_heredoc", O_RDONLY);
	if (fd < 0)
		return (perror("minishell: heredoc read failed"), -1);
	dup2(fd, STDIN_FILENO);
	close(fd);
	unlink("/tmp/minishell_heredoc");
	return (0);
}

void execute_redirection(char **tokens, t_shell *shell)
{
    int i;
	int	j;
    char **cmd;
    int cmd_index;

	i = 0;
	j = 0;
	cmd = calloc(1024, sizeof(char *));
	cmd_index = 0;
    if (!cmd)
    {
        perror("malloc failed");
        exit(1);
    }
	while(tokens[j] != NULL)
	{
		if (strcmp(tokens[j], "|") == 0)
        {
            tokens[j] = NULL;
            execute_piped_commands(tokens, shell);
            free(cmd);
            return;
        }
		j++; 
	}
    while (tokens[i] != NULL)
    {
        if (tokens[i] && strcmp(tokens[i], "<") == 0)
        {
            if (tokens[i + 1] == NULL)
            {
                fprintf(stderr, "minishell: syntax error: missing file for input redirection\n");
                free(cmd);
                exit(1);
            }
            handle_input_redirection(tokens, &i);
            i++;
        }
        else if (tokens[i] && (strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], ">>") == 0))
        {
            if (tokens[i + 1] == NULL)
            {
                fprintf(stderr, "minishell: syntax error: missing file for output redirection\n");
                free(cmd);
                exit(1);
            }
            handle_output_redirection(tokens, &i, strcmp(tokens[i], ">>") == 0);
            i++;
        }
        else if (tokens[i] && strcmp(tokens[i], "<<") == 0)
        {
            if (tokens[i + 1] == NULL)
            {
                fprintf(stderr, "minishell: syntax error: missing delimiter for heredoc\n");
                free(cmd);
                exit(1);
            }
            handle_heredoc(tokens[i + 1]);
            i += 2;
        }
        else
        {
            cmd[cmd_index++] = strdup(tokens[i]);
            i++;
        }
    }
    cmd[cmd_index] = NULL;
    if (is_builtin(cmd[0]))
    {
		if (handle_redirections(tokens) < 0)
		{
			printf("Redirection failed!\n");
			return ;
		}
		execute_builtin(cmd, shell);
    }
    else
    {
        ft_execvp(cmd[0], cmd, shell->env);
        perror("execvp failed");
    }
    for (int j = 0; j < cmd_index; j++)
        free(cmd[j]);
    free(cmd);
    exit(1);
}

int handle_redirections(char **tokens)
{
    int i = 0;
    int fd;

    while (tokens[i])
    {
        if (strcmp(tokens[i], ">") == 0)  // Handle output redirection
        {
            fd = open(tokens[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd < 0)
            {
                perror("open failed");
                return -1;
            }
            printf("Redirecting STDOUT from %d to %d\n", STDOUT_FILENO, fd);
            dup2(fd, STDOUT_FILENO);
            close(fd);
            // Remove `> file` from tokens
            while (tokens[i])
            {
                tokens[i] = tokens[i + 2];
                i++;
            }
            i = -1;  // Reset i to re-check remaining tokens
        }
        else if (strcmp(tokens[i], "<") == 0)  // Handle input redirection
        {
            fd = open(tokens[i + 1], O_RDONLY);
            if (fd < 0)
            {
                perror("open failed");
                return -1;
            }
            printf("Redirecting STDIN from %d to %d\n", STDIN_FILENO, fd);
            dup2(fd, STDIN_FILENO);
            close(fd);
            // Remove `< file` from tokens
            while (tokens[i])
            {
                tokens[i] = tokens[i + 2];
                i++;
            }
            i = -1;
        }
        i++;
    }
    return 0;
}
