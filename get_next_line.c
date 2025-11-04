/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpinheir <rpinheir@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 11:43:36 by rpinheir          #+#    #+#             */
/*   Updated: 2025/11/04 15:55:11 by rpinheir         ###   ########.fr       */
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

	i = 0;
	while (!ft_strchr(stash, '\n'))
	{
		read(fd, buffer, BUFFER_SIZE);
		ft_strlcat(stash, buffer, BUFFER_SIZE);
	}
}

void	clean_stash(char *stash)
{
	int	len;
	int	i;

	len = ft_strchr(stash, '\n');
	i = 0;
	while (stash[i])
	{
		stash[i] = stash[i + len];
		i++;
	}
	free(stash);
}

char	*get_next_line(int fd)
{
	static char	*stash;
	char		*line;
	char		buffer[BUFFER_SIZE];
	int			len;

	stash = malloc(sizeof(char) * BUFFER_SIZE);
	if (!stash)
		return (NULL);
	if (BUFFER_SIZE <= 0 | read(fd, buffer, 0) == -1)
		return (NULL);
	while (read(fd, buffer, BUFFER_SIZE) > 0)
	{
		read_to_stash(fd, stash);
		if (!stash)
			break ;
		len = ft_strchr(stash, '\n');
		ft_strlcpy(line, stash, len);
		clean_stash(stash);
		free(stash);
	}
	free(stash);
	return (line);
}

int	main(void)
{
	int	fd;

	fd = open("text.txt", O_RDONLY);
	while (get_next_line(fd) != NULL)
		printf("%s", get_next_line(fd));
}
