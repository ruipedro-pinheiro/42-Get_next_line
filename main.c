/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rpinheir <rpinheir@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 20:30:00 by rpinheir          #+#    #+#             */
/*   Updated: 2025/11/12 21:50:43 by rpinheir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	main(void)
{
	int		fd;
	char	*output;

	fd = open("hello.txt", O_RDONLY);
	output = get_next_line(fd);
	while (output != NULL)
	{
		printf("%s", output);
		free(output);
		output = get_next_line(fd);
	}
	close(fd);
	return (0);
}
