/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unarchive.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguiulfo <gguiulfo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/27 23:06:30 by gguiulfo          #+#    #+#             */
/*   Updated: 2018/01/28 00:34:08 by gguiulfo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_tar.h"

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

int main(int argc, char const *argv[])
{
	FILE	*fp;

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
	int file_size = ftell(fp);
	if (file_size < HEADER_SIZE)
	{
		dprintf(STDERR, FT_UNARCHIVE_ERR1);
		exit(EXIT_FAILURE);
	}
	// fseek(fp, 0, SEEK_SET);
	// int file_size = ftell(fp);
	rewind(fp);
	printf("%d\n", file_size);

	char *data = (char *)malloc(sizeof(char) * file_size);
	fread(data, file_size, sizeof(data), fp);
	// printf("%s\n", data);
	// while (data + offset)
	// {
	// char buf[13];
	// strncpy(buf, data + 124, 12);
	// int fsize = atoi(buf);
	// printf("%d\n", fsize);

	// write(1, data + HEADER_SIZE, fsize);
		// write(1, data + 124, 12);
		// write(1, data, HEADER_SIZE);
		// offset += HEADER_SIZE;
	// }
	unsigned int offset = 0;
	while (offset < file_size)
	{
		char buf[13];
		strncpy(buf, data + offset + 124, 12);
		int fsize = atoi(buf);
		printf("%d\n", fsize);
		char namebuf[101];
		strncpy(namebuf, data + offset, 100);
		if (namebuf[0])
			unarchive_file(namebuf, data + offset + HEADER_SIZE, fsize);
		offset += ((fsize / HEADER_SIZE) + 1) * HEADER_SIZE;
		offset += HEADER_SIZE;
		printf("offset: %d\n", offset);
	}
	// for (int i = 0; i < file_size; i++)
	// {
	// 	printf("%c\n", data[i]);
	// }
	// while (*data)
	// {
		// printf("%s\n", data);
	// }
	fclose(fp);
	return (0);
}
