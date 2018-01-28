/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   archive.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/27 16:43:36 by asyed             #+#    #+#             */
/*   Updated: 2018/01/28 05:05:26 by asyed            ###   ########.fr       */
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
	// else if (S_ISIFO(buf.st_mode))
	// 	(*tar_h)->linkflag = '6';
	else if (S_ISREG(buf.st_mode))
		(*tar_h)->linkflag = '0';
	else if (S_ISDIR(buf.st_mode))
	{
		(*tar_h)->linkflag = '5';
		snprintf((*tar_h)->size, 12, "%011o ", 0);
	}
	else if (S_ISLNK(buf.st_mode))
		(*tar_h)->linkflag = '2';
	else
	{
		DBG("Failed linkflag()");
		return (0);
	}
	return (1);
}

/*
** Need to do proper base conversions lol
** Do sym/hard link support.
** Need to fix the space after the size to appear and make a cleaner 
** way of doing so
**
** Need chksum support.
** Add initial trailing '/' to the directory at first. 
*/

int	add_stats(int fd, t_tarheader **tar_h)
{
	struct stat	buf;
	void 		*tempstruct;
	char		*tmp;

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
	if (!(tmp = calloc(sizeof(char), 14)))
		return (0);
	snprintf(tmp, 13, "%011llo ", buf.st_size);
	DBG("TMP = \"%s\"", tmp);
	memcpy((*tar_h)->size, tmp, 12);
	free(tmp);
	DBG("Size = \"%s\" vs %lld", (*tar_h)->size, buf.st_size);
	// strcpy((*tar_h)->mtime, ft_itoa(buf.st_mtime));
	snprintf((*tar_h)->mtime, 12, "%lo", buf.st_mtime);
	DBG("mtime = %s", (*tar_h)->mtime);
	strcpy((*tar_h)->indicator, "ustar");
	DBG("");
	memcpy((*tar_h)->version, "00", 2);
	// sprintf((*tar_h)->version, "00");
	DBG("");
	linkflag(tar_h, buf);
	DBG("LinkFlag = %c", (*tar_h)->linkflag);
	sprintf((*tar_h)->devicemajor, "%06o ", major(buf.st_rdev));
	sprintf((*tar_h)->deviceminor, "%06o ", minor(buf.st_rdev));
	// snprintf((*tar_h)->deviceminor, "%06o ", minor(buf.st_dev));
	tempstruct = getpwuid(buf.st_uid);
	DBG("");
	sprintf((*tar_h)->uname, "%s", ((struct passwd *)tempstruct)->pw_name);
	DBG("");
	tempstruct = getgrgid(buf.st_gid);
	sprintf((*tar_h)->gname, "%s", ((struct group *)tempstruct)->gr_name);
	//Calculate checksum? 

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
		{
			DBG("Failed to calloc(prefix)");
			return (0);
		}
		if (ft_dstr_new(*prefix, 20))
		{
			DBG("Failed to make the dynamic string!");
			return (0);
		}
		// ft_dstr_append(prefix, filename);
		DBG("Made prefix!");
	}
	else
	{
		DBG("Prefix already existed.");
	}
	new_name = calloc(sizeof(char), (*prefix)->len + ft_strlen(filename) + 1);
	new_name = strcat(new_name, (*prefix)->data);
	new_name = strcat(new_name, filename);
	if (!(name = opendir(new_name)))
	{
		DBG("File = %s failed (%s)", new_name, strerror(errno));
		return (0);
	}
	ft_dstr_append(*prefix, filename);
	ft_dstr_append(*prefix, "/");
	while ((dir = readdir(name)))
	{
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, ".."))
		{
			DBG("add_file(%s)", dir->d_name);
			if (!add_file(destfile, dir->d_name, prefix))
				DBG("File = %s readdir(%s)", filename, strerror(errno));
		}
		// return (0);
	}
	return (1);
}

int	write_file(FILE *destfile, FILE *src)
{
	size_t	size;
	char	*buf;

	fseek(src, 0, SEEK_END);
	size = ftell(src);
	rewind(src);

	if (!(buf = calloc(sizeof(char), size)))
		return (0);
	fread(buf, 1, size, src);
	fwrite(buf, 1, size, destfile);
	fclose(src);
	free(buf);
	// Padding
	if (size > 512 && (size % 512))
	{
		size = size % 512;
		if (!(buf = calloc(sizeof(char), size)))
			return (0);
		fwrite(buf, 1, size, destfile);
		free(buf);
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
		DBG("Prefix = %s for filename = %s", (*prefix)->data, new_name);
	}
	else
		new_name = filename;
	if (!strcpy(tar_h->name, new_name))
	{
		printf("Failed to copy name\n");
		return (0);
	}
	if (!(file = fopen(new_name, "r")))
	{
		printf("Failed to open %s\n", new_name);
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
	if (!write_file(destfile, file))
		return (0);
	fclose(file);
	if (tar_h->linkflag == '5')
	{
		DBG("Directory name = %s", filename);
		if (prefix && *prefix)
		{
			if (!(save = calloc(sizeof(t_dstr), 1)) && ft_dstr_new(save, (*prefix)->cap))
				return (0);
			ft_dstr_append(save, (*prefix)->data);
			add_directory(destfile, filename, prefix);
			DBG("Prefix restored from %s -> %s", (*prefix)->data, save->data);
			(*prefix) = save;
		}
		else
		{
			add_directory(destfile, filename, prefix);
			*prefix = NULL;
		}
	}
	// if (!copy_contents(destfile, file))
	// {
	// 	printf("Error; copy_contents()\n");
	// 	return (0);
	// }
	// fwrite(tar_h, sizeof(t_tarheader), ,destfile)
	return (1);
}

// int	handle_file(FILE *destfile, char *filename)
// {
// 	t_tarheader	*tar_h;
// 	FILE		*file;

// 	if (!(tar_h = calloc(sizeof(t_tarheader), 1)))
// 	{
// 		printf("Failed to calloc(sof(header), %s)\n", filename);
// 		return (0);
// 	}
// 	if (!strcpy(tar_h->name, filename))
// 	{
// 		printf("Failed to copy name\n");
// 		return (0);
// 	}
// 	if (!(file = fopen(filename, "r")))
// 	{
// 		printf("Failed to open %s\n", filename);
// 		return (0);
// 	}
// 	else
// 		printf("fd = %d\n", fileno(file));

// }

int	main(int argc, char *argv[])
{
	t_tarheader	*tar_h;
	void		*test;
	FILE		*destfile;
	int			i;
	// t_dstr		*prefix;

	// if (!prefix = calloc(sizeof(t_dstr), 1))
	// {
	// 	DBG("Failed %s", strerror(errno));
	// 	return (0);
	// }
	// if (!ft_dstr_new(prefix, 20))
	// 	return (0);
	destfile = fopen(argv[1], "w"); //We can check if the file already exists later.
	i = 2;
	while (i < argc)
	{
		// if (!handle_file(destfile, argv[i++]))
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
	fwrite(test, 1, 1024, destfile);
	fclose(destfile);
}