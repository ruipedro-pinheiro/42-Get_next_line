/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpinheir <rpinheir@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 11:43:36 by rpinheir          #+#    #+#             */
/*   Updated: 2025/11/14 21:00:02 by rpinheir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

/*
** Lit le fichier et accumule les données dans le stash jusqu'à trouver un '\n'
** @param fd: file descriptor à lire
** @param stash: pointeur vers le stash où accumuler les données lues
** Fonction: Continue de lire BUFFER_SIZE octets à la fois et les ajoute
** au stash jusqu'à trouver un '\n' ou atteindre EOF
*/
void	read_to_stash(int fd, char **stash)
{
	char	*buffer;
	int		bytes_readed;
	char	*temp_stash;

	buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buffer)
		return (free(*stash), *stash = NULL, (void)0);
	while (ft_strchr(*stash, '\n') == -1)
	{
		bytes_readed = read(fd, buffer, BUFFER_SIZE);
		if (bytes_readed == -1)
			return (free(buffer), free(*stash), *stash = NULL, (void)0);
		if (bytes_readed == 0)
			break ;
		buffer[bytes_readed] = '\0';
		temp_stash = ft_strjoin(*stash, buffer);
		if (!temp_stash)
			return (free(buffer), free(*stash), *stash = NULL, (void)0);
		free(*stash);
		*stash = temp_stash;
	}
	free(buffer);
}

/*
** Nettoie le stash en supprimant la ligne qui vient d'être extraite
** @param stash: le stash à nettoyer
** Fonction: Après avoir extrait une ligne, on déplace tout ce qui vient
** APRÈS le '\n' au début du stash, pour garder le reste pour le prochain appel
** Exemple: si stash = "ligne1\nligne2", après clean -> stash = "ligne2"
*/
void	clean_stash(char *stash)
{
	int	len;
	int	i;

	len = ft_strchr(stash, '\n');
	i = 0;
	while (stash[len + 1 + i])
	{
		stash[i] = stash[len + 1 + i];
		i++;
	}
	stash[i] = '\0';
}

/*
** Initialise un stash vide
** @param stash: paramètre non utilisé (juste pour la signature)
** @return: un pointeur vers un stash vide (string vide allouée)
** Fonction: Alloue 1 octet pour créer une string vide "",
** qui servira de point de départ pour accumuler les lectures
*/
char	*malloc_stash(char *stash)
{
	stash = malloc(sizeof(char) * 1);
	if (!stash)
		return (NULL);
	stash[0] = '\0';
	return (stash);
}

char	*extract_line(char **stash, int len)
{
	char	*line;

	if (len == -1)
	{
		if (!*stash || !(*stash)[0])
			return (free(*stash), *stash = NULL, NULL);
		line = ft_strdup(*stash);
		return (free(*stash), *stash = NULL, line);
	}
	line = malloc(sizeof(char) * (len + 2));
	if (line == NULL)
		return (free(*stash), *stash = NULL, NULL);
	ft_strlcpy(line, *stash, len + 2);
	clean_stash(*stash);
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*stash;
	int			len;

	if (BUFFER_SIZE <= 0 || fd < 0)
		return (NULL);
	if (stash == NULL)
	{
		stash = malloc_stash(stash);
		if (!stash)
			return (NULL);
	}
	read_to_stash(fd, &stash);
	if (!stash)
		return (NULL);
	len = ft_strchr(stash, '\n');
	return (extract_line(&stash, len));
}
