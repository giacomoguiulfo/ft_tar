/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_archive.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/27 16:43:36 by asyed             #+#    #+#             */
/*   Updated: 2018/01/29 00:33:55 by asyed            ###   ########.fr       */
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
		if (((*tar_h)->name)[strlen((*tar_h)->name) - 1] != '/')
			strcat((*tar_h)->name, "/");
	}
	else if (S_ISLNK(buf.st_mode))
		(*tar_h)->linkflag = '2';
	else
		return (0);
	return (1);
}

int	add_directory(FILE *destfile, char *filename, t_dstr **prefix)
{
	struct dirent	*dir;
	DIR				*name;
	char			*new_name;

	if (!*prefix)
	{
		if (!(*prefix = calloc(sizeof(t_dstr), 1)))
			return (0);
		if (ft_dstr_new(*prefix, 20))
			return (0);
	}
	new_name = calloc(sizeof(char), (*prefix)->cap + strlen(filename) + 1);
	memcpy(new_name, (*prefix)->data, (*prefix)->len);
	new_name = strcat(new_name, filename);
	if (!(name = opendir(new_name)))
		return (0);
	ft_dstr_append(*prefix, filename);
	if (((*prefix)->data)[(*prefix)->len - 1] != '/')
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
	if (size > 0)
	{
		if (!(buf = calloc(sizeof(char), size)))
			return (0);
		fread(buf, size, 1, src);
		fwrite(buf, size, 1, destfile);
		fclose(src);
		free(buf);
	}
	if ((size > 512 && size % 512) || !size)
	{
		size = size % 512;
		fwrite(pad, 512 - size, 1, destfile);
	}
	return (1);
}

int	add_file(FILE *destfile, char *filename, t_dstr **prefix)
{
	t_tarheader	*tar_h;
	FILE		*file;
	char		*new_name;

	if (!(tar_h = calloc(sizeof(t_tarheader), 1)))
		return (0);
	if (prefix && *prefix)
	{
		new_name = calloc(sizeof(char), (*prefix)->cap + strlen(filename) + 1);
		memcpy(new_name, (*prefix)->data, (*prefix)->len);
		new_name = strcat(new_name, filename);
	}
	else
		new_name = filename;
	if (!strcpy(tar_h->name, new_name) || !(file = fopen(new_name, "r"))
		|| !add_stats(fileno(file), &tar_h))
		return (0);
	fwrite(tar_h, sizeof(t_tarheader), 1, destfile);
	if (tar_h->linkflag != '5' && !write_file(destfile, file, tar_h))
		return (0);
	fclose(file);
	return (linkf_handle(tar_h->linkflag, destfile, prefix, filename));
}

int	ft_tar(int argc, char *argv[], FILE *destfile)
{
	void		*test;
	int			i;

	i = 0;
	test = NULL;
	while (argv[i])
	{
		if (!add_file(destfile, argv[i++], (t_dstr **)&test))
		{
			printf("%s: Error %s\n", g_argv[0], strerror(errno));
			return (0);
		}
	}
	return (end_padding(&test, destfile));
}
