/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unarchive.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguiulfo <gguiulfo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/27 23:06:30 by gguiulfo          #+#    #+#             */
/*   Updated: 2018/01/28 22:56:25 by gguiulfo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_tar.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include <time.h>

char	**g_argv;

#define FTAR_ERR(s, ...)	((fprintf(stderr, s, ##__VA_ARGS__)) ? 1 : 1)
#define FTAR_USAGE			"usage: %s archived_file", g_argv[0]
#define FTAR_ERR_MSG		"%s: %s: %s", g_argv[0]
#define FTAR_ERR_MSG1		"%s: Unrecognized archive format", g_argv[0]
#define FTAR_ERR_MSG2		"%s: %s", g_argv[0]
#define FTAR_HEADSIZE		(512)
#define FTAR_HEADALIGN(x)	((((x) / FTAR_HEADSIZE) + 1) * FTAR_HEADSIZE)
#define FTAR_NORMAL(x)		(*((x) + 156) == '0')
#define FTAR_LINK(x)		((*((x) + 156) == '1') || (*((x) + 156) == '2'))
#define FTAR_ISSPECIAL(x)	((*((x) + 156) == '3') || (*((x) + 156) == '4'))
#define FTAR_ISDIR(x)		(*((x) + 156) == '5')
#define FTAR_FILESIZE(d, o)	((d) + (o) + 124)
#define FTAR_MODE(d, o)		((d) + (o) + 100)

static void unarchive_file(const char *path, char data[], int size)
{
	FILE	*fp;

	if (!(fp = fopen(path, "w")))
	{
		perror(g_argv[0]);
		return ;
	}
	fwrite(data, size, sizeof(char), fp);
	fclose(fp);
}



void		unarchive_special(const char *path, char data[], int size)
{
	char	buf[16];
	mode_t	mode;
	dev_t	dev;
	size_t	major;
	size_t	minor;

	strncpy(buf, data + 100, 9);
	mode = strtol(buf, NULL, 8);
	major = strtoul(data + 329, NULL, 8);
	minor = strtoul(data + 337, NULL, 8);
	dev = makedev(major, minor);
	if (mknod(path, mode, dev))
		perror(g_argv[0]);
	else
		unarchive_file(path, data, size);
}

static void	ftar_permissions(const char *path, char *data)
{
	struct utimbuf	ubuf;
	char			buf[156];
	uid_t			uid;
	gid_t			gid;
	mode_t			mode;

	strncpy(buf, data + 108,  9);
	uid = strtol(buf, NULL, 8);
	strncpy(buf, data + 116,  9);
	gid = strtol(buf, NULL, 8);
	strncpy(buf, data + 100, 9);
	mode = strtol(buf, NULL, 8);
	strncpy(buf, data + 136, 13);
	ubuf.modtime = strtol(buf, NULL, 8);
	time(&ubuf.actime);
	chown(path, uid, gid);
	chmod(path, mode);
	if (utime(path, &ubuf))
		perror(g_argv[0]);
}

int		validate_checksum(t_tarheader *tar_h, char *original)
{
    size_t                    	chk_pre;
    size_t                    	i;
    unsigned char            	*bytes;
	char						buf[8];

    chk_pre = 0;
    i = 0;
    bytes = (unsigned char *)(tar_h);
    memset(tar_h->checksum, ' ', 8);
    while (i < sizeof(t_tarheader))
        chk_pre += bytes[i++];
	snprintf(buf, 7, "%06zo", chk_pre);
	return (strcmp(buf, original));
}

static void	ft_unarchive(char *data, size_t archive_size)
{
	t_tarheader tar_h;
	char	buf[156];
	size_t	offset;
	size_t	file_size;

	offset = 0;
	while (offset < archive_size)
	{
		strncpy(buf, data + offset + 148, 8);
		memcpy(&tar_h, data + offset, FTAR_HEADSIZE);
		if (validate_checksum(&tar_h, buf))
			return ;
		strncpy(buf, FTAR_FILESIZE(data, offset), 13);
		file_size = strtoul(buf, (char **) NULL, 8);
		strncpy(buf, data + offset, 101);
		if (FTAR_NORMAL(data + offset))
			unarchive_file(buf, data + offset + FTAR_HEADSIZE, file_size);
		else if (FTAR_LINK(data + offset))
			; // handle links
		else if (FTAR_ISSPECIAL(data + offset))
			unarchive_special(buf, data + offset + FTAR_HEADSIZE, file_size);
		else if (FTAR_ISDIR(data + offset))
		{
			if (-1 == mkdir(buf, 0) && errno != EEXIST)
				perror(g_argv[0]);
		}
		else if (buf[0]) // fdsafdsa
			FTAR_ERR(FTAR_ERR_MSG1);
		// else if (0 == getuid())
		// else if ()... handle links
		// else invalid file type
		// if (1 || 0 == getuid()) // FTAR_HAS_OPT_LP
		// {
		// 	ftar_permissions(buf, data + offset);
		// 	// ftar_time();
		// }
		if (file_size > 0)
			offset += FTAR_HEADALIGN(file_size);
		offset += FTAR_HEADSIZE;
	}
}


int main(int argc, char const *argv[])
{
	FILE	*fp;
	size_t	archive_size;
	char	*data;

	g_argv = (char **)argv;
	if (argc != 2)
		return (FTAR_ERR(FTAR_USAGE));
	if (!(fp = fopen(argv[1], "r")))
		return (FTAR_ERR(FTAR_ERR_MSG, argv[1], strerror(errno)));
	fseek(fp, 0, SEEK_END);
	archive_size = ftell(fp);
	if (archive_size < FTAR_HEADSIZE)
		return (FTAR_ERR(FTAR_ERR_MSG1));
	rewind(fp);
	data = (char *)malloc(sizeof(char) * archive_size);
	fread(data, archive_size, sizeof(char), fp);
	ft_unarchive(data, archive_size);
	fclose(fp);
	return (0);
}
