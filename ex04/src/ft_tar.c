/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tar.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/28 23:23:26 by gguiulfo          #+#    #+#             */
/*   Updated: 2018/01/29 19:22:42 by asyed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_tar.h"
#include <errno.h>

char	**g_argv;

int		get_file(char *def, char *arg, t_tar *data)
{
	if (!data)
		return (1);
	if (!(data->file = strdup(arg)))
		return (1);
	(void)def;
	return (0);
}

int		gzip_parse(char **cmd, t_tar data)
{
	if (!(*cmd = calloc(sizeof(char), strlen(data.file) + 10)))
		return (1);
	strcpy(*cmd, "gzip ");
	if (TAR_HAS_OPT_LX(data.flags))
	{
		strcat(*cmd, "-d ");
		strcat(*cmd, data.file);
		data.file[strlen(data.file) - 3] = '\0';
		return (system(*cmd));
	}
	else
		strcat(*cmd, data.file);
	return (0);
}

int		flags_parse(t_tar *data, FILE **fp, int *tar, char **cmd)
{
	if (TAR_HAS_OPT_LC(data->flags) && TAR_HAS_OPT_LT(data->flags))
		return (TAR_ERR("Can't specify both -c and -t"));
	if (TAR_HAS_OPT_LC(data->flags) && TAR_HAS_OPT_LX(data->flags))
		return (TAR_ERR("Can't specify both -c and -x"));
	if (TAR_HAS_OPT_LT(data->flags) && TAR_HAS_OPT_LX(data->flags))
		return (TAR_ERR("Can't specify both -t and -x"));
	*tar = TAR_HAS_OPT_LC(data->flags);
	if (TAR_HAS_OPT_LZ(data->flags) && gzip_parse(cmd, *data))
		return (1);
	if (TAR_HAS_OPT_LF(data->flags))
	{
		if (!TAR_HAS_OPT_LC(data->flags) && !TAR_HAS_OPT_LT(data->flags) &&
			!TAR_HAS_OPT_LX(data->flags))
			return (TAR_ERR("Must specify one of -c, -t, -x"));
		if (!(*fp = fopen(data->file, (*tar) ? "w" : "r")))
			return (TAR_ERR("%s: %s", data->file, strerror(errno)));
	}
	else if (!(*fp = fdopen((*tar) ? 1 : 0, (*tar) ? "w" : "r")))
		return (TAR_ERR("%s", strerror(errno)));
	return (0);
}

int		main(int argc __attribute__((unused)), char const *argv[])
{
	t_tar	data;
	FILE	*fp;
	char	*cmd;
	int		tar;

	g_argv = (char **)argv;
	data.flags = 0;
	if (ft_opts((char **)argv, &g_taropts, &data, 1))
		return (1);
	if (flags_parse(&data, &fp, &tar, &cmd))
		return (1);
	if (TAR_HAS_OPT_LZ(data.flags) && TAR_HAS_OPT_LC(data.flags))
	{
		if (!ft_tar(&data, fp))
			return (system(cmd));
	}
	return ((tar) ? ft_tar(&data, fp) : ft_untar(fp));
}
