/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tar.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/28 05:05:32 by asyed             #+#    #+#             */
/*   Updated: 2018/01/29 19:23:06 by asyed            ###   ########.fr       */
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
# define TAR_MSG(s, ...)		"%s: " s "\n", g_argv[0], ##__VA_ARGS__
# define TAR_PRN(s, ...)		fprintf(stderr, TAR_MSG(s, ##__VA_ARGS__))
# define TAR_ERR(s, ...)		(TAR_PRN(s, ##__VA_ARGS__) ? 1 : 1)
# define TAR_OPT_LC			(1 << 0)
# define TAR_OPT_LF			(1 << 1)
# define TAR_OPT_LP			(1 << 2)
# define TAR_OPT_LT			(1 << 3)
# define TAR_OPT_LV			(1 << 4)
# define TAR_OPT_LX			(1 << 5)
# define TAR_OPT_LZ			(1 << 6)
# define TAR_HAS_OPT_LC(x)	((x) & TAR_OPT_LC)
# define TAR_HAS_OPT_LF(x)	((x) & TAR_OPT_LF)
# define TAR_HAS_OPT_LP(x)	((x) & TAR_OPT_LP)
# define TAR_HAS_OPT_LT(x)	((x) & TAR_OPT_LT)
# define TAR_HAS_OPT_LV(x)	((x) & TAR_OPT_LV)
# define TAR_HAS_OPT_LX(x)	((x) & TAR_OPT_LX)
# define TAR_HAS_OPT_LZ(x)	((x) & TAR_OPT_LZ)

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

typedef struct	s_tar
{
	t_flag		flags;
	char		**argv;
	char		*file;
}				t_tar;

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
int				add_file(FILE *destfile, char *filename, t_dstr **prefix,
						uint8_t verbose);
int				ft_tar(t_tar *data, FILE *destfile);

/*
** unutils.c
*/

int				validate_checksum(t_tarheader *tar_h, char *original);
void			unarchive_special(const char *path, char data[], int size);

/*
** ft_unarchive.c
*/

int				ft_unarchive(char *data, size_t archive_size);
int				file_handle(char *data, size_t offset,
							size_t file_size, char *buf);
void			ftar_permissions(const char *path, char *data);
void			unarchive_file(const char *path, char data[], int size);
int				ft_untar(FILE *fp);

/*
** ft_tar.c
*/

int				get_file(char *def, char *arg, t_tar *data);

static t_optsdata	g_taropts =
{
	"ft_tar", "ft_tar - manipulate tape archives\n", "", NULL, 1, 1, 0, {
		{'c', NULL, "Create a new archive containing the specified items.",
			NULL, NULL, TAR_OPT_LC, 0, NULL, 0, 0},
		{'f', NULL, "Read/write the archive from/to the specified file.",
			NULL, "file", TAR_OPT_LF, 0, &get_file, 1, 0},
		{'p', NULL, "(x mode only) Preserve file permissions.",
			NULL, NULL, TAR_OPT_LP, 0, NULL, 0, 0},
		{'t', NULL, "List archive contents to stdout.",
			NULL, NULL, TAR_OPT_LT, 0, NULL, 0, 0},
		{'v', NULL, "Produce verbose output.",
			NULL, NULL, TAR_OPT_LV, 0, NULL, 0, 0},
		{'x', NULL, "Extract to disk from the archive.",
			NULL, NULL, TAR_OPT_LX, 0, NULL, 0, 0},
		{'z', NULL, "Use GZIP(1) compression.",
			NULL, NULL, TAR_OPT_LZ, 0, NULL, 0, 0},
		{0, NULL, NULL, NULL, NULL, 0, 0, NULL, 0, 0}
	}
};

extern char **g_argv;
#endif
