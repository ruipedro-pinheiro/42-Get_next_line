/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpinheir <rpinheir@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 11:43:36 by rpinheir          #+#    #+#             */
/*   Updated: 2025/11/05 11:27:00 by rpinheir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

void	read_to_stash(int fd, char *stash)
{
	int		i;
	char	buffer[BUFFER_SIZE];
	int		len;

	len = ft_strlen(stash);
	i = 0;
	while (ft_strchr(stash, '\n') || read(fd, buffer, 0) > 0)
	{
		read(fd, buffer, BUFFER_SIZE);
		ft_strlcat(stash, buffer, len);
	}
}

void	clean_stash(char *stash)
{
	int	len;
	int	i;

	len = ft_strchr(stash, '\n');
	i = 0;
	while (i != len)
	{
		stash[i] = '\0';
	}
	i = 0;
	while (stash[i])
	{
		stash[i + len] = stash[i];
		i++;
	}
}

char	*get_next_line(int fd)
{
	static char	*stash;
	char		*line;
	char		buffer[BUFFER_SIZE];
	int			len;
	int			size;

	if (BUFFER_SIZE <= 0 | read(fd, buffer, 0) == -1)
		return (NULL);
	if (stash == NULL)
	{
		stash = malloc(sizeof(char) * BUFFER_SIZE);
		if (!stash)
			return (NULL);
	}
	size = ft_strlen(stash);
	if (size == BUFFER_SIZE)
	{
		stash = malloc(malloc(sizeof(char) * BUFFER_SIZE));
	}
	read_to_stash(fd, stash);
	len = ft_strchr(stash, '\n');
	line = malloc(sizeof(char) * len + 2);
	if (line == NULL)
		return (NULL);
	ft_strlcpy(line, stash, len);
	clean_stash(stash);
	if (read(fd, buffer, 0) > 1)
		free(stash);
	return (line);
}

int	main(void)
{
	int	fd;

	fd = open("text.txt", O_RDONLY);
}
