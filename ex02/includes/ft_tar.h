/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tar.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/28 05:05:32 by asyed             #+#    #+#             */
/*   Updated: 2018/01/29 00:03:51 by asyed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_TAR_H
# define FT_TAR_H
# include <stdlib.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include "libft.h"

# define ERR_PARAMS "Not enough paramaters"

typedef struct	s_tarheader
{
	char		name[100];
	char		mode[8];
	char		uid[8];
	char		gid[8];
	char		size[12];
	char		mtime[12];
	char		checksum[8];
	char		linkflag;
	char		linkname[100];
	char		indicator[6];
	char		version[2];
	char		uname[32];
	char		gname[32];
	char		devicemajor[8];
	char		deviceminor[8];
	char		nameprefix[155];
	char		pad[12];
}				t_tarheader;

/*
** utils.c
*/

int				end_padding(void **test, FILE *destfile);
int				linkf_handle(char linkflag, FILE *destfile,
							t_dstr **prefix, char *filename);
void			ft_suffixnull(char *dest, const char *format,
							int64_t str, size_t len);
void			calc_chksum(t_tarheader **tar_h);
int				add_stats(int fd, t_tarheader **tar_h);

/*
** ft_archive.c
*/

int				linkflag(t_tarheader **tar_h, struct stat buf);
int				add_directory(FILE *destfile, char *filename,
							t_dstr **prefix);
int				write_file(FILE *destfile, FILE *src, t_tarheader *tar_h);
int				add_file(FILE *destfile, char *filename, t_dstr **prefix);
int				ft_tar(int argc, char *argv[], FILE *destfile);

/*
** unutils.c
*/

int			validate_checksum(t_tarheader *tar_h, char *original);
void		unarchive_special(const char *path, char data[], int size);

/*
** ft_unarchive.c
*/

int			ft_unarchive(char *data, size_t archive_size);
int			file_handle(char *data, size_t offset, size_t file_size, char *buf);
void		ftar_permissions(const char *path, char *data);
void		unarchive_file(const char *path, char data[], int size);
int			ft_untar(int argc, char const *argv[], FILE *fp);

#endif
