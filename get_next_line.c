/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpinheir <rpinheir@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 11:43:36 by rpinheir          #+#    #+#             */
/*   Updated: 2025/11/04 17:40:35 by rpinheir         ###   ########.fr       */
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
	read(fd, buffer, BUFFER_SIZE);
	ft_strlcat(stash, buffer, BUFFER_SIZE);
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
}

char	*get_next_line(int fd)
{
	static char	*stash;
	char		*line;
	char		buffer[BUFFER_SIZE];
	int			len;

	if (BUFFER_SIZE <= 0 | read(fd, buffer, 0) == -1)
		return (NULL);
	// check si stash est deja initialiser (avec deja du contenu dessus)
	stash = malloc(sizeof(char) * BUFFER_SIZE); // a determiner la taille
	if (!stash)
		return (NULL);
	read_to_stash(fd, stash);
	len = ft_strchr(stash, '\n');
	ft_strlcpy(line, stash, len);
	clean_stash(stash);
	free(stash);
	return (line);
}

int	main(void)
{
	int fd;

	fd = open("text.txt", O_RDONLY);
