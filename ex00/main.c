/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/27 16:43:36 by asyed             #+#    #+#             */
/*   Updated: 2018/01/27 19:23:10 by asyed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_tar.h"
#include <errno.h>
/*
** Need to do proper base conversions lol
** Do sym/hard link support.
*/

int	add_stats(int fd, t_tarheader *tar_h)
{
	struct stat *buf;

	if (fstat(fd, buf) == -1)
	{
		printf("Failed to fstat %s\n", strerror(errno));
		return (0);
	}
	strcpy(tar_h->mode, ft_itoa(buf->st_mode));
	printf("tar_h->mode = %s (== %d)\n", tar_h->mode, buf->st_mode);
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
	FILE		*file;

	if (!(tar_h = calloc(sizeof(t_tarheader), 1)))
	{
		printf("Failed to calloc(sof(header), %s)\n", filename);
		return (0);
	}
	if (!strcpy(tar_h->name, filename))
	{
		printf("Failed to copy name\n");
		return (0);
	}
	if (!(file = fopen(filename, "r")))
	{
		printf("Failed to open %s\n", filename);
		return (0);
	}
	else
		printf("fd = %d\n", fileno(file));
	if (!add_stats(fileno(file), tar_h))
	{
		printf("Failed to add stats\n");
		return (0);
	}
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