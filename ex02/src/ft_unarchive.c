/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unarchive.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguiulfo <gguiulfo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/27 23:06:30 by gguiulfo          #+#    #+#             */
/*   Updated: 2018/01/29 00:26:04 by gguiulfo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_tar.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include <time.h>

extern char	**g_argv;

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
#define USER_IS_ROOT		(0 == getuid())

void	unarchive_file(const char *path, char data[], int size)
{
	FILE	*fp;

	if (!(fp = fopen(path, "w")))
	{
		perror(g_argv[0]);
		return ;
	}
	fwrite(data, size, sizeof(char), fp);
	fclose(fp);
	printf("%s, ", path);
}

void	ftar_permissions(const char *path, char *data)
{
	struct utimbuf	ubuf;
	char			buf[156];
	uid_t			uid;
	gid_t			gid;
	mode_t			mode;

	strncpy(buf, data + 108, 9);
	uid = strtol(buf, NULL, 8);
	strncpy(buf, data + 116, 9);
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

int	file_handle(char *data, size_t offset, size_t file_size, char *buf)
{
	if (FTAR_NORMAL(data + offset))
		unarchive_file(buf, data + offset + FTAR_HEADSIZE, file_size);
	else if (FTAR_ISSPECIAL(data + offset))
		unarchive_special(buf, data + offset + FTAR_HEADSIZE, file_size);
	else if (FTAR_ISDIR(data + offset))
	{
		if (-1 == mkdir(buf, 0) && errno != EEXIST)
			return (FTAR_ERR(FTAR_ERR_MSG2, strerror(errno)));
		printf("%s, ", buf);
	}
	else if (buf[0])
		return (FTAR_ERR(FTAR_ERR_MSG1));
	return (1);
}

int	ft_unarchive(char *data, size_t archive_size)
{
	t_tarheader tar_h;
	char		buf[156];
	size_t		offset;
	size_t		file_size;

	offset = 0;
	while (offset < archive_size)
	{
		strncpy(buf, data + offset + 148, 8);
		memcpy(&tar_h, data + offset, FTAR_HEADSIZE);
		if (buf[0] && validate_checksum(&tar_h, buf))
			return (FTAR_ERR(FTAR_ERR_MSG2, "Invalid checksum"));
		strncpy(buf, FTAR_FILESIZE(data, offset), 13);
		file_size = strtoul(buf, (char **)NULL, 8);
		strncpy(buf, data + offset, 101);
		if (!file_handle(data, offset, file_size, buf))
			return (0);
		if (buf[0] && (1 || USER_IS_ROOT))
			ftar_permissions(buf, data + offset);
		if (file_size > 0)
			offset += FTAR_HEADALIGN(file_size);
		offset += FTAR_HEADSIZE;
	}
	return (0);
}

int	ft_untar(char **argv, FILE *fp)
{
	size_t	archive_size;
	char	*data;

	if (!fp)
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
