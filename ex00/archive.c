/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   archive.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/27 16:43:36 by asyed             #+#    #+#             */
/*   Updated: 2018/01/28 00:10:26 by asyed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_tar.h"
#include <errno.h>
#include <grp.h>
#include <pwd.h>

/*
** Need to do proper base conversions lol
** Do sym/hard link support.
*/

int	add_stats(int fd, t_tarheader **tar_h)
{
	struct stat buf;
	void *tempstruct;

	if (fstat(fd, &buf) == -1)
	{
		printf("Failed to fstat %s\n", strerror(errno));
		return (0);
	}
	sprintf((*tar_h)->mode, "%06o ", (buf.st_mode & 0777));
	DBG("Mode = %s", (*tar_h)->mode);
	sprintf((*tar_h)->uid, "%06o ", buf.st_uid);
	DBG("UID = %s", (*tar_h)->uid);
	sprintf((*tar_h)->gid, "%06o ", buf.st_gid);
	DBG("GID = %s", (*tar_h)->gid);
	snprintf((*tar_h)->size, 12, "%010llo", buf.st_size);
	DBG("Size = %s", (*tar_h)->size);
	// strcpy((*tar_h)->mtime, ft_itoa(buf.st_mtime));
	snprintf((*tar_h)->mtime, 12, "%lo", buf.st_mtime);
	DBG("mtime = %s", (*tar_h)->mtime);
	strcpy((*tar_h)->indicator, "ustar");
	DBG("");
	memcpy((*tar_h)->version, "00", 2);
	// sprintf((*tar_h)->version, "00");
	DBG("");
	tempstruct = getpwuid(buf.st_uid);
	DBG("");
	sprintf((*tar_h)->uname, "%s", ((struct passwd *)tempstruct)->pw_name);
	DBG("");
	tempstruct = getgrgid(buf.st_gid);
	sprintf((*tar_h)->gname, "%s", ((struct group *)tempstruct)->gr_name);
	//Calculate checksum? 
	return (1);
}

// int	copy_contents(FILE *destfile, FILE *src)
// {
// 	char buf;

// }

/*
** Take advantage of the prefix field in the future if 
** the name is larger than 100 chars.
*/

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