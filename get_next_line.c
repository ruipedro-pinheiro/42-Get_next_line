/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpinheir <rpinheir@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 11:43:36 by rpinheir          #+#    #+#             */
/*   Updated: 2025/11/12 17:42:24 by rpinheir         ###   ########.fr       */
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

	buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));	// FIX: +1 pour '\0'
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
		buffer[bytes_readed] = '\0';	// FIX: ajouté - null-terminate buffer
		temp_stash = ft_strjoin(*stash, buffer);
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
	while (stash[len + 1 + i])	// FIX: était while (i < len)
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

/*
** NOUVELLE FONCTION: Extrait une ligne du stash
** @param stash: pointeur vers le stash contenant les données lues
** @param len: position du '\n' dans le stash (ou -1 si pas de '\n' = EOF)
** @return: la ligne extraite (avec '\n' si trouvé) ou NULL si erreur/fin
** Fonction: Deux cas possibles:
**   1. len == -1 (pas de '\n'): c'est la dernière ligne du fichier (EOF)
**      -> retourne tout le stash et le libère
**   2. len >= 0 (il y a un '\n'): extrait la ligne jusqu'au '\n' inclus
**      -> retourne la ligne et nettoie le stash en gardant le reste
*/
char	*extract_line(char **stash, int len)	// NOUVELLE FONCTION créée
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
		return (NULL);
	ft_strlcpy(line, *stash, len + 2);
	clean_stash(*stash);
	return (line);
}

/*
** FONCTION PRINCIPALE: Lit et retourne la prochaine ligne d'un file descriptor
** @param fd: file descriptor à lire
** @return: la prochaine ligne (avec '\n' si présent) ou NULL si fin/erreur
**
** LOGIQUE GLOBALE:
** 1. Le stash (static) persiste entre les appels pour garder les données en trop
** 2. On lit par blocs de BUFFER_SIZE jusqu'à trouver un '\n'
** 3. On extrait la ligne jusqu'au '\n' (inclus)
** 4. On garde le reste dans le stash pour le prochain appel
**
** Exemple avec BUFFER_SIZE=5 et fichier "Hello\nWorld\n":
** - 1er appel: lit "Hello", trouve '\n', retourne "Hello\n", stash=""
** - 2e appel: lit "World", trouve '\n', retourne "World\n", stash=""
** - 3e appel: plus rien à lire, retourne NULL
*/
char	*get_next_line(int fd)
{
	static char	*stash;
	int			len;

	if (BUFFER_SIZE <= 0 || fd < 0)
		return (NULL);
	if (read(fd, 0, 0) < 0)	// FIX: meilleure gestion d'erreur
	{
		if (stash)
			free(stash);
		stash = NULL;
		return (NULL);
	}
	if (stash == NULL)
	{
		stash = malloc_stash(stash);
		if (!stash)
			return (NULL);
	}
	read_to_stash(fd, &stash);
	len = ft_strchr(stash, '\n');
	return (extract_line(&stash, len));	// FIX: utilise fonction extraite
}
