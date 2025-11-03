/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpinheir <rpinheir@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 11:43:36 by rpinheir          #+#    #+#             */
/*   Updated: 2025/11/03 13:13:03 by rpinheir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h";

char	*get_next_line(int fd)
{
	t_list	*stash;
	t_list	*line;
	int		i;

	i = 0;
	line = (void *)0;
	if (BUFFER_SIZE <= 0 | read(fd, 0, 0) < 0)
		return ((void *)0);
	read_to_stash(fd, stash);
	// lire depuis fd vers buffer (read) tant BUFFER_SIZE bytes
	// lire tant qu'on trouve pas '\n' dans stash
	// compter combien de bytes on a lu jusqua '\n'
	// extraire ces memes bytes dans line ('\n' compris)
	// loop tant qu'on est pas a EOF (read == 0 || read == -1)
	// extraire le reste de stash dans line
	// clean + free stash
}

void	read_to_stash(int fd, char *stash)
{
	int i;
	char buffer[1024];
	i = 0;
	read(fd, buffer, BUFFER_SIZE);
	while (stash[i] || stash[i + 1] != '\n')
	{
		stash[i] = buffer[i];
		i++;
	}
}