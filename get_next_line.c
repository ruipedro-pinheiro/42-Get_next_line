/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpinheir <rpinheir@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 11:43:36 by rpinheir          #+#    #+#             */
/*   Updated: 2025/11/04 14:31:44 by rpinheir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h";

char	*get_next_line(int fd)
{
	static char	*stash;
	char		*line;
	char		*buf;
	int			len;

	line = (void *)0;
	if (BUFFER_SIZE <= 0 | read(fd, 0, 0) <= 0)
		return ((void *)0);
	while (read(fd, buf, BUFFER_SIZE) != 0)
	{
		read_to_stash(fd, stash);
		len = ft_strchr(stash, '\n');
		ft_strlcpy(line, stash, len); // extraire le reste de stash dans line
		// clean la stash et laisser le reste
		clean_stash(stash);
	}
	// clean + free stash
}

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
}
