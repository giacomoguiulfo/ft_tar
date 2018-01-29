/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unutils.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/28 23:28:57 by asyed             #+#    #+#             */
/*   Updated: 2018/01/28 23:40:31 by asyed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_tar.h"

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

int			validate_checksum(t_tarheader *tar_h, char *original)
{
	size_t			chk_pre;
	size_t			i;
	unsigned char	*bytes;
	char			buf[8];

	chk_pre = 0;
	i = 0;
	bytes = (unsigned char *)(tar_h);
	memset(tar_h->checksum, ' ', 8);
	while (i < sizeof(t_tarheader))
		chk_pre += bytes[i++];
	snprintf(buf, 7, "%06zo", chk_pre);
	return (strcmp(buf, original));
}
