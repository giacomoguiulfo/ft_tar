/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unarchive.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguiulfo <gguiulfo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/27 23:06:30 by gguiulfo          #+#    #+#             */
/*   Updated: 2018/01/28 01:26:30 by gguiulfo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_tar.h"
#include <sys/stat.h>
#include <sys/types.h>

#define FT_UNARCHIVE_USAGE	"usage: %s archived_file"
#define HEADER_SIZE			(512)
#define FT_UNARCHIVE_ERR1	"Unrecognized archive format"

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
	unsigned int offset;

	offset = 0;
	while (offset < archive_size)
	{
		char buf[13];
		strncpy(buf, data + offset + 124, 12);
		int fsize = atoi(buf);
		printf("%d\n", fsize);
		char namebuf[101];
		strncpy(namebuf, data + offset, 100);
		if (*(data + offset + 156) == '5')
			mkdir(namebuf, S_IRWXU | S_IRWXG | S_IRWXO);
		else if (namebuf[0])
			unarchive_file(namebuf, data + offset + HEADER_SIZE, fsize);
		if (fsize > 0)
			offset += ((fsize / HEADER_SIZE) + 1) * HEADER_SIZE;
		offset += HEADER_SIZE;
	}
}

int main(int argc, char const *argv[])
{
	FILE	*fp;
	size_t	archive_size;
	char	*data;

	if (argc != 2)
	{
		dprintf(STDERR, FT_UNARCHIVE_USAGE, argv[0]);
		exit(EXIT_FAILURE);
	}
	if (!(fp = fopen(argv[1], "r")))
	{
		perror(argv[0]);
		exit(EXIT_FAILURE);
	}
	fseek(fp, 0, SEEK_END);
	archive_size = ftell(fp);
	if (archive_size < HEADER_SIZE)
	{
		dprintf(STDERR, FT_UNARCHIVE_ERR1);
		exit(EXIT_FAILURE);
	}
	rewind(fp);
	data = (char *)malloc(sizeof(char) * archive_size);
	fread(data, archive_size, sizeof(data), fp);
	ft_unarchive(data, archive_size);
	fclose(fp);
	return (0);
}
