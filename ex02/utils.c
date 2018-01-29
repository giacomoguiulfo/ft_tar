/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/28 21:43:24 by asyed             #+#    #+#             */
/*   Updated: 2018/01/28 23:40:23 by asyed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_tar.h"
#include <errno.h>
#include <pwd.h>
#include <grp.h>

int		end_padding(void **test, FILE *destfile)
{
	if (destfile)
	{
		if (*test)
			free(*test);
		if (!(*test = calloc(sizeof(char), 1024)))
			return (0);
		fwrite(*test, 1024, 1, destfile);
		fclose(destfile);
		printf("-> done\n");
		return (1);
	}
	return (0);
}

int		linkf_handle(char linkflag, FILE *destfile,
						t_dstr **prefix, char *filename)
{
	t_dstr	*save;

	save = NULL;
	if (linkflag == '5')
	{
		if (prefix && *prefix)
		{
			if (!(save = calloc(sizeof(t_dstr), 1))
				&& ft_dstr_new(save, (*prefix)->cap))
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

void	ft_suffixnull(char *dest, const char *format,
							int64_t str, size_t len)
{
	static char	large[20] = "";

	snprintf(large, len + 1, format, str);
	memcpy(dest, large, len);
}

void	calc_chksum(t_tarheader **tar_h)
{
	size_t					chk_pre;
	size_t					i;
	unsigned char			*bytes;

	chk_pre = 0;
	i = 0;
	bytes = (unsigned char *)(*tar_h);
	memset((*tar_h)->checksum, ' ', 8);
	while (i < sizeof(t_tarheader))
		chk_pre += bytes[i++];
	snprintf((*tar_h)->checksum, 7, "%06zo", chk_pre);
}

int		add_stats(int fd, t_tarheader **tar_h)
{
	struct stat	buf;
	void		*tempstruct;

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
	sprintf((*tar_h)->devicemajor, "%06o ", major(buf.st_dev));
	sprintf((*tar_h)->deviceminor, "%06o ", minor(buf.st_dev));
	tempstruct = getpwuid(buf.st_uid);
	snprintf((*tar_h)->uname, 32, "%s", ((struct passwd *)tempstruct)->pw_name);
	tempstruct = getgrgid(buf.st_gid);
	snprintf((*tar_h)->gname, 32, "%s", ((struct group *)tempstruct)->gr_name);
	calc_chksum(tar_h);
	return (1);
}
