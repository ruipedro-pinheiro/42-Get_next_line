/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpinheir <rpinheir@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 11:43:36 by rpinheir          #+#    #+#             */
/*   Updated: 2025/11/12 13:30:25 by rpinheir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	read_to_stash(int fd, char *stash)
{
	char	buffer[BUFFER_SIZE];
	int		bytes_readed;
	char	*temp_stash;

	while (ft_strchr(stash, '\n'))
	{
		bytes_readed = read(fd, buffer, BUFFER_SIZE);
		if (bytes_readed <= 0)
			break ;
		temp_stash = ft_strjoin(stash, buffer);
		free(stash);
		stash = temp_stash;
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
		stash[i] = stash[len + 1];
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