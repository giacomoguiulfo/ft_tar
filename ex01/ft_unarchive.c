/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unarchive.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguiulfo <gguiulfo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/27 23:06:30 by gguiulfo          #+#    #+#             */
/*   Updated: 2018/01/28 03:30:56 by gguiulfo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_tar.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

char	**g_argv;

#define FTAR_ERR(s, ...)	((fprintf(stderr, s, ##__VA_ARGS__)) ? 1 : 1)
#define FTAR_USAGE			"usage: %s archived_file", g_argv[0]
#define FTAR_ERR_MSG		"%s: %s: %s", g_argv[0]
#define FTAR_ERR_MSG1		"Unrecognized archive format"
#define FTAR_HEADSIZE		(512)
#define FTAR_HEADALIGN(x)	((((x) / FTAR_HEADSIZE) + 1) * FTAR_HEADSIZE)
#define FTAR_ISDIR(d, o)	(*((d) + (o) + 156) == '5')
#define FTAR_FILESIZE(d, o)	((d) + (o) + 124)

static void unarchive_file(char *filename, char data[], int size)
{
	FILE	*fp;

	if (!(fp = fopen(filename, "w")))
	{
		perror("ft_unarchive");
		return ;
	}
	fwrite(data, size, sizeof(char), fp);
	fclose(fp);
}

static void	ft_unarchive(char *data, size_t archive_size)
{
	char	buf[156];
	size_t	offset;
	size_t	file_size;

	offset = 0;
	while (offset < archive_size)
	{
		strncpy(buf, FTAR_FILESIZE(data, offset), 13);
		file_size = strtoul(buf, (char **) NULL, 8);
		strncpy(buf, data + offset, 101);
		if (FTAR_ISDIR(data, offset))
			mkdir(buf, S_IRWXU | S_IRWXG | S_IRWXO);
		else if (buf[0])
			unarchive_file(buf, data + offset + FTAR_HEADSIZE, file_size);
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
	fread(data, archive_size, sizeof(data), fp);
	ft_unarchive(data, archive_size);
	fclose(fp);
	return (0);
}
