/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/27 16:43:36 by asyed             #+#    #+#             */
/*   Updated: 2018/01/27 18:53:14 by asyed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_tar.h"

/*
** Need to do proper base conversions lol
** Do sym/hard link support.
*/

int	add_stats(int fd, t_tarheader *tar_h)
{
	struct stat *buf;

	if (fstat(fd, buf) == -1)
		return (0);
	// tar_h->mode = buf->st_mode;
	// tar_h->uid = buf->st_uid;
	// tar_h->gid = buf->st_gid;
	// tar_h->size = buf->st_size;
	// tar_h->mtime = buf->st_mtime;
	//Calculate checksum? 
	return (1);
}

int	add_file(FILE *destfile, char *filename)
{
	t_tarheader	*tar_h;
	char		buf[1024];

	if (!(tar_h = calloc(sizeof(t_tarheader), 1)))
	{
		printf("Failed to calloc(sof(header), %s)\n", filename);
		return (0);
	}
	if (!strcpy(tar_h->name, filename))
		return (0);
	if (!add_stats(fileno(destfile), tar_h))
		return (0);
	// fwrite(tar_h, sizeof(t_tarheader), ,destfile)
	return (1);
}

int	main(int argc, char *argv[])
{
	t_tarheader	*tar_h;
	FILE		*destfile;
	int			i;

	destfile = fopen(argv[1], "w"); //We can check if the file already exists later.
	i = 2;
	while (i < argc)
	{
		if (!add_file(destfile, argv[i++]))
		{
			printf("Error\n");
			return (0);
		}
	}
}