/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   archive.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/27 16:43:36 by asyed             #+#    #+#             */
/*   Updated: 2018/01/27 23:01:44 by asyed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_tar.h"
#include <errno.h>
/*
** Need to do proper base conversions lol
** Do sym/hard link support.
*/

int	add_stats(int fd, t_tarheader **tar_h)
{
	struct stat buf;

	if (fstat(fd, &buf) == -1)
	{
		printf("Failed to fstat %s\n", strerror(errno));
		return (0);
	}
	strcpy((*tar_h)->mode, ft_itoa(buf.st_mode & 0777));
	DBG("Mode = %o", buf.st_mode & 07777);
	strcpy((*tar_h)->uid, ft_itoa(buf.st_uid));
	strcpy((*tar_h)->gid, ft_itoa(buf.st_gid));
	strcpy((*tar_h)->size, ft_itoa(buf.st_size));
	strcpy((*tar_h)->mtime, ft_itoa(buf.st_mtime));
	//Calculate checksum? 
	return (1);
}

// int	copy_contents(FILE *destfile, FILE *src)
// {
// 	char buf;

// }

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
	if (!add_stats(fileno(file), &tar_h))
	{
		printf("Failed to add stats\n");
		return (0);
	}
	// printf("FD: %d\n", fileno(file));
	DBG("FD: %d %d", fileno(destfile), sizeof(t_tarheader));
	DBG("Mode = %s", tar_h->mode);
	if (write(fileno(destfile), tar_h, sizeof(t_tarheader)) == -1)
		printf("Failed to write(%s)\n", strerror(errno));
	// if (!copy_contents(destfile, file))
	// {
	// 	printf("Error; copy_contents()\n");
	// 	return (0);
	// }
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