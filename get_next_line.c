/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpinheir <rpinheir@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 11:43:36 by rpinheir          #+#    #+#             */
/*   Updated: 2025/11/12 12:11:35 by rpinheir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	read_to_stash(int fd, char *stash)
{
	char	buffer[BUFFER_SIZE];
	int		len;

	len = ft_strlen(stash);
	while (ft_strchr(stash, '\n') || read(fd, buffer, 0) > 0)
	{
		read(fd, buffer, BUFFER_SIZE);
		ft_strlcpy(stash, buffer, len);
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

void	stash_realloc(char *stash)
{
	char	*temp;
	int		len;

	temp = 0;
	len = ft_strlen(stash);
	if (BUFFER_SIZE > BUFFER_SIZE + len)
	{
		ft_strlcpy(temp, stash, len);
		free(stash);
		stash = malloc(sizeof(char) * BUFFER_SIZE + len + 1);
		ft_strlcpy(stash, temp, len);
	}
}

char	*get_next_line(int fd)
{
	static char	*stash;
	char		*line;
	char		buffer[BUFFER_SIZE];
	int			len;

	if (BUFFER_SIZE <= 0 || read(fd, 0, 0) <= 0)
		return (NULL);
	if (stash == NULL)
	{
		stash = malloc(sizeof(char) * BUFFER_SIZE);
		if (!stash)
			return (NULL);
	}
	read_to_stash(fd, stash);
	len = ft_strchr(stash, '\n');
	stash_realloc(stash);
	line = malloc(sizeof(char) * len + 2);
	if (line == NULL)
		return (NULL);
	ft_strlcpy(line, stash, len);
	clean_stash(stash);
	if (read(fd, buffer, 0) > 1)
		free(stash);
	return (line);
}
/**
int	main(void)
{
	int	fd;

	fd = open("text.txt", O_RDONLY);
}
**/