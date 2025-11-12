/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpinheir <rpinheir@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 11:43:36 by rpinheir          #+#    #+#             */
/*   Updated: 2025/11/12 15:18:15 by rpinheir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	read_to_stash(int fd, char **stash)
{
	char	*buffer;
	int		bytes_readed;
	char	*temp_stash;

	buffer = malloc(sizeof(char) * BUFFER_SIZE);
	if (!buffer)
		return ;
	while (ft_strchr(*stash, '\n') == -1)
	{
		bytes_readed = read(fd, buffer, BUFFER_SIZE);
		if (bytes_readed <= 0)
		{
			free(buffer);
			return ;
		}
		buffer[bytes_readed] = '\0';
		temp_stash = ft_strjoin(*stash, buffer);
		free(*stash);
		*stash = temp_stash;
	}
	free(buffer);
}

void	clean_stash(char *stash)
{
	int	len;
	int	i;

	len = ft_strchr(stash, '\n');
	i = 0;
	while (i < len)
	{
		stash[i] = stash[len + 1 + i];
		i++;
	}
	stash[i] = '\0';
}

char	*malloc_stash(char *stash)
{
	stash = malloc(sizeof(char) * BUFFER_SIZE);
	if (!stash)
		return (NULL);
	stash[0] = '\0';
	return (stash);
}

char	*get_next_line(int fd)
{
	static char	*stash;
	char		*line;
	int			len;

	if (BUFFER_SIZE <= 0 || read(fd, 0, 0) <= 0)
		return (NULL);
	if (stash == NULL)
	{
		stash = malloc_stash(stash);
	}
	read_to_stash(fd, &stash);
	len = ft_strchr(stash, '\n');
	line = malloc(sizeof(char) * len + 2);
	if (line == NULL)
		return (NULL);
	ft_strlcpy(line, stash, len + 2);
	clean_stash(stash);
	return (line);
}
