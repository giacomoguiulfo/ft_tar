/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tar.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/28 05:05:32 by asyed             #+#    #+#             */
/*   Updated: 2018/01/28 05:05:46 by asyed            ###   ########.fr       */
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

// # define DEVICEMAJOR(x) (((x >> 8) & 0xfff) | (unsigned int) (x >> 32) & ~0xfff)

int	add_file(FILE *destfile, char *filename, t_dstr **prefix);

/*
** File Size: Pad with zeroes instead of spaces...1988 uses spaces.
** 	For historical reasons, a final NUL or space character should also be used.
** 	Thus although there are 12 bytes reserved for storing the file size,
** 	only 11 octal digits can be stored.
**
*/

typedef struct	s_tarheader
{
	char 		name[100];
	char 		mode[8];
	char 		uid[8];
	char 		gid[8];
	char 		size[12];
	char 		mtime[12];
	char 		checksum[8];
	char 		linkflag;
	char 		linkname[100];
	char 		indicator[6];
	char 		version[2];
	char		uname[32];
	char		gname[32];
	char		devicemajor[8];
	char		deviceminor[8];
	char		nameprefix[155];
	char 		pad[255];
} 				t_tarheader;

#endif
