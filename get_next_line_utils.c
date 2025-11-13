/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpinheir <rpinheir@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 11:27:32 by rpinheir          #+#    #+#             */
/*   Updated: 2025/11/13 14:00:48 by rpinheir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

/*
** Calcule la longueur d'une string
** @param s: la string à mesurer
** @return: le nombre de caractères (sans compter le '\0')
*/
int	ft_strlen(const char *s)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i] != '\0')
	{
		i++;
	}
	return (i);
}

/*
** Cherche un caractère dans une string et retourne sa position
** @param s: la string à parcourir
** @param c: le caractère à chercher
** @return: la position du caractère (ou -1 si non trouvé)
** Note: Cette version retourne l'INDEX, pas un pointeur comme strchr standard
*/
int	ft_strchr(const char *s, int c)
{
	int	i;

	if (!s)
		return (-1);
	i = 0;
	while (s[i])
	{
		if (s[i] == (char)c)
			return (i);
		i++;
	}
	if ((char)c == '\0')
		return (i);
	return (-1);
}

/*
** Copie une string dans une autre avec limite de taille
** @param dst: destination
** @param src: source à copier
** @param size: taille maximale (incluant le '\0')
** @return: la longueur de la string source
*/
int	ft_strlcpy(char *dst, const char *src, int size)
{
	int	i;
	int	len;

	i = 0;
	len = ft_strlen(src);
	if (size == 0)
		return (len);
	while (i < (size - 1) && src[i] != '\0')
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (len);
}

/*
** Duplique une string (alloue de la mémoire et copie le contenu)
** @param s: la string à dupliquer
** @return: un pointeur vers la nouvelle string allouée
*/
char	*ft_strdup(const char *s)
{
	char	*ptr;
	size_t	len;

	len = ft_strlen(s) + 1;
	ptr = malloc(len);
	if (!ptr)
		return (NULL);
	ft_strlcpy(ptr, s, len);
	return (ptr);
}

/*
** Concatène deux strings en allouant une nouvelle string
** @param s1: première string
** @param s2: deuxième string
** @return: un pointeur vers la nouvelle string contenant s1+s2
** Note: Gère les cas NULL pour s1 ou s2
*/
char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*ptr;
	int		len1;
	int		len2;

	if (s1 == NULL)
		return (ft_strdup(s2));
	if (s2 == NULL)
		return (ft_strdup(s1));
	if (s1 == NULL && s2 == NULL)
		return (NULL);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	ptr = malloc(len1 + len2 + 1);
	if (!ptr)
		return ((void *)0);
	ft_strlcpy(&ptr[0], s1, len1 + 1);
	ft_strlcpy(&ptr[len1], s2, len2 + 1);
	ptr[len1 + len2] = '\0';
	return (ptr);
}
