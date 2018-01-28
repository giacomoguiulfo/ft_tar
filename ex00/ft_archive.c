/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_archive.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: suedadam <suedadam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/27 16:43:36 by asyed             #+#    #+#             */
/*   Updated: 2018/01/28 15:54:00 by suedadam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_tar.h"
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <sys/types.h>
#include <dirent.h>

int	linkflag(t_tarheader **tar_h, struct stat buf)
{
	if (S_ISBLK(buf.st_mode))
		(*tar_h)->linkflag = '4';
	else if (S_ISCHR(buf.st_mode))
		(*tar_h)->linkflag = '3';
	else if (S_ISREG(buf.st_mode))
		(*tar_h)->linkflag = '0';
	else if (S_ISDIR(buf.st_mode))
	{
		(*tar_h)->linkflag = '5';
		ft_suffixnull((*tar_h)->size, "%011llo ", 0, 12);
		strcat((*tar_h)->name, "/");
	}
	else if (S_ISLNK(buf.st_mode))
		(*tar_h)->linkflag = '2';
	else
	{
		// DBG("Failed linkflag()");
		return (0);
	}
	return (1);
}

static void	ft_suffixnull(char *dest, const char *format, int64_t str, size_t len)
{
	static char	large[20] = "";

	snprintf(large, len + 1, format, str);
	memcpy(dest, large, len);
}

static void	calc_chksum(t_tarheader **tar_h)
{
	size_t			chk_pre;
	int				i;
	unsigned char	*bytes;

	i = 0;
	bytes = (unsigned char *)(*tar_h);
	memset((*tar_h)->checksum, ' ', 8);
	while (i < sizeof(t_tarheader))
		chk_pre += bytes[i++];
	sprintf((*tar_h)->checksum, "%06zo", chk_pre);
}

/*
** Need to do proper base conversions lol
** Do sym/hard link support.
** Add initial trailing '/' to the directory at first. 
*/

int	add_stats(int fd, t_tarheader **tar_h)
{
	struct stat	buf;
	void 		*tempstruct;

	if (fstat(fd, &buf) == -1)
	{
		printf("Failed to fstat %s\n", strerror(errno));
		return (0);
	}
	sprintf((*tar_h)->mode, "%06o ", (buf.st_mode & 0777));
	sprintf((*tar_h)->uid, "%06o ", buf.st_uid);
	sprintf((*tar_h)->gid, "%06o ", buf.st_gid);
	ft_suffixnull((*tar_h)->size, "%011llo ", buf.st_size, 12);
	ft_suffixnull((*tar_h)->mtime, "%011lo ", buf.st_mtime, 12);
	strcpy((*tar_h)->indicator, "ustar");
	memcpy((*tar_h)->version, "00", 2);
	linkflag(tar_h, buf);
	sprintf((*tar_h)->devicemajor, "%06o ", major(buf.st_rdev));
	sprintf((*tar_h)->deviceminor, "%06o ", minor(buf.st_rdev));
	tempstruct = getpwuid(buf.st_uid);
	sprintf((*tar_h)->uname, "%s", ((struct passwd *)tempstruct)->pw_name);
	tempstruct = getgrgid(buf.st_gid);
	sprintf((*tar_h)->gname, "%s", ((struct group *)tempstruct)->gr_name);
	calc_chksum(tar_h);
	return (1);
}

int	add_directory(FILE *destfile, char *filename, t_dstr **prefix)
{
	struct dirent 	*dir;
	DIR				*name;
	char			*new_name;

	if (!*prefix)
	{
		if (!(*prefix = calloc(sizeof(t_dstr), 1)))
			return (0);
		if (ft_dstr_new(*prefix, 20))
			return (0);
	}
	new_name = calloc(sizeof(char), (*prefix)->len + ft_strlen(filename) + 1);
	new_name = strcat(new_name, (*prefix)->data);
	new_name = strcat(new_name, filename);
	if (!(name = opendir(new_name)))
		return (0);
	ft_dstr_append(*prefix, filename);
	ft_dstr_append(*prefix, "/");
	while ((dir = readdir(name)))
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, ".."))
			if (!add_file(destfile, dir->d_name, prefix))
				return (0);
	return (1);
}

int	write_file(FILE *destfile, FILE *src, t_tarheader *tar_h)
{
	size_t		size;
	static char	pad[512];
	char		size_buf[13];
	char		*buf;

	strncpy(size_buf, tar_h->size, 11);
	size = strtoul(size_buf, NULL, 8);
	DBG("Size_buf = %s an size = %zd", size_buf, size);
	if (size > 0)
	{
		if (!(buf = calloc(sizeof(char), size)))
			return (0);
		fread(buf, size, 1, src);
		fwrite(buf, size, 1, destfile);
		fclose(src);
		free(buf);
	}
	// printf("Size = %zu", size);
	if ((size > 512 && size % 512) || !size)
	{
		size = size % 512;
		fwrite(pad, 512 - size, 1, destfile);
		printf("Padded with %zu byts\n", 512 - size);
	}
	return (1);
}

/*
** Take advantage of the prefix field in the future if 
** the name is larger than 100 chars.
*/

int	add_file(FILE *destfile, char *filename, t_dstr **prefix)
{
	t_tarheader	*tar_h;
	char		buf[1024];
	FILE		*file;
	char		*new_name;
	t_dstr		*save;

	if (!(tar_h = calloc(sizeof(t_tarheader), 1)))
	{
		printf("Failed to calloc(sof(header), %s)\n", filename);
		return (0);
	}
	if (prefix && *prefix)
	{
		new_name = calloc(sizeof(char), (*prefix)->len + ft_strlen(filename) + 1);
		new_name = strcat(new_name, (*prefix)->data);
		new_name = strcat(new_name, filename);
	}
	else
		new_name = filename;
	if (!strcpy(tar_h->name, new_name))
	{
		// DBG("Failed to copy name");
		return (0);
	}
	if (!(file = fopen(new_name, "r")))
	{
		// DBG("Failed to open %s", new_name);
		return (0);
	}
	if (!add_stats(fileno(file), &tar_h))
		return (0);
	// DBG("Writing header for file = %s", tar_h->name);
	fwrite(tar_h, sizeof(t_tarheader), 1, destfile);
	// DBG("Wrote header for file = %s", tar_h->name);
	if (!write_file(destfile, file, tar_h))
		return (0);
	printf("a %s\n", new_name);
	fclose(file);
	if (tar_h->linkflag == '5')
	{
		if (prefix && *prefix)
		{
			if (!(save = calloc(sizeof(t_dstr), 1)) && ft_dstr_new(save, (*prefix)->cap))
				return (0);
			ft_dstr_append(save, (*prefix)->data);
			add_directory(destfile, filename, prefix);
			(*prefix) = save;
		}
		else
		{
			add_directory(destfile, filename, prefix);
			*prefix = NULL;
		}
	}
	return (1);
}

int	main(int argc, char *argv[])
{
	t_tarheader	*tar_h;
	void		*test;
	FILE		*destfile;
	int			i;

	destfile = fopen(argv[1], "w");
	i = 2;
	while (i < argc)
	{
		if (!add_file(destfile, argv[i++], (t_dstr **)&test))
		{
			printf("Error\n");
			return (0);
		}
	}
	if (test)
		free(test);
	if (!(test = calloc(sizeof(char), 1024)))
		return (0);
	fwrite(test, 1024, 1, destfile);
	fclose(destfile);
}
