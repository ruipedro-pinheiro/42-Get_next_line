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
	// TODO: Je lis seulement UNE fois ! Il faut lire en BOUCLE jusqu'a trouver '\n' ou EOF
	// Tant que ft_strchr(stash, '\n') retourne 0 (pas trouve), continuer de lire
	// Verifier aussi que read() retourne > 0 (sinon EOF)
	read(fd, buffer, BUFFER_SIZE);
	// TODO: ft_strlcat a besoin de la taille TOTALE du buffer de destination, pas BUFFER_SIZE
	ft_strlcat(stash, buffer, BUFFER_SIZE);
}

void	clean_stash(char *stash)
{
	int	len;
	int	i;

	// TODO: ft_strchr retourne la POSITION de '\n', mais je dois decaler de position+1
	// Car je veux supprimer tout JUSQU'AU '\n' inclus
	len = ft_strchr(stash, '\n');
	i = 0;
	// TODO: Je dois copier depuis stash[len+1] vers stash[0]
	// et continuer jusqu'a '\0'
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
	// TODO MAJEUR: Je REALLOC stash a CHAQUE appel ! Une variable static doit persister
	// Solution: Verifier si stash == NULL (premier appel), alors allouer. Sinon, garder l'ancien stash
	// TODO: La taille doit etre dynamique, pas juste BUFFER_SIZE (je vais lire plusieurs fois)
	stash = malloc(sizeof(char) * BUFFER_SIZE); // a determiner la taille
	if (!stash)
		return (NULL);
	read_to_stash(fd, stash);
	len = ft_strchr(stash, '\n');
	// TODO CRITIQUE: line n'est PAS alloue ! Je dois faire malloc AVANT de copier dedans
	// Allouer len+2 (pour '\n' + '\0')
	ft_strlcpy(line, stash, len);
	clean_stash(stash);
	// TODO: NE PAS FREE stash ! Il doit persister pour le prochain appel (static)
	// Free seulement a la fin du fichier (quand read retourne 0 ou erreur)
	free(stash);
	return (line);
}

int	main(void)
{
	int fd;

	fd = open("text.txt", O_RDONLY);
