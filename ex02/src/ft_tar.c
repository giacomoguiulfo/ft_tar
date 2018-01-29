/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tar.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/28 23:23:26 by gguiulfo          #+#    #+#             */
/*   Updated: 2018/01/29 01:01:40 by asyed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_tar.h"
#include <errno.h>

char	**g_argv;

#define TAR_OPT_LC			(1 << 0)
#define TAR_OPT_LF			(1 << 1)
#define TAR_OPT_LP			(1 << 2)
#define TAR_OPT_LT			(1 << 3)
#define TAR_OPT_LV			(1 << 4)
#define TAR_OPT_LX			(1 << 5)
#define TAR_HAS_OPT_LC(x)	((x) & TAR_OPT_LC)
#define TAR_HAS_OPT_LF(x)	((x) & TAR_OPT_LF)
#define TAR_HAS_OPT_LP(x)	((x) & TAR_OPT_LP)
#define TAR_HAS_OPT_LT(x)	((x) & TAR_OPT_LT)
#define TAR_HAS_OPT_LV(x)	((x) & TAR_OPT_LV)
#define TAR_HAS_OPT_LX(x)	((x) & TAR_OPT_LX)

static t_optsdata	g_taropts =
{
	"ft_tar", NULL, NULL, NULL, 0, 1, 0, {
		{'c', NULL, NULL, NULL, NULL, TAR_OPT_LC, 0, NULL, 0, 0},
		{'f', NULL, NULL, NULL, NULL, TAR_OPT_LF, 0, NULL, 0, 0},
		{'p', NULL, NULL, NULL, NULL, TAR_OPT_LP, 0, NULL, 0, 0},
		{'t', NULL, NULL, NULL, NULL, TAR_OPT_LT, 0, NULL, 0, 0},
		{'v', NULL, NULL, NULL, NULL, TAR_OPT_LV, 0, NULL, 0, 0},
		{'x', NULL, NULL, NULL, NULL, TAR_OPT_LX, 0, NULL, 0, 0},
		{0, NULL, NULL, NULL, NULL, 0, 0, NULL, 0, 0}
	}
};

int		main(int argc __attribute__((unused)), char const *argv[])
{
	t_optparser	data;
	FILE		*fp;
	int			tar;

	g_argv = (char **)argv;
	data.flags = 0;
	if (ft_opts((char **)argv, &g_taropts, &data, 1))
		return (1);
	if (TAR_HAS_OPT_LC(data.flags) && TAR_HAS_OPT_LT(data.flags))
		return (TAR_ERR("Can't specify both -c and -t\n"));
	if (TAR_HAS_OPT_LC(data.flags) && TAR_HAS_OPT_LX(data.flags))
		return (TAR_ERR("Can't specify both -c and -x\n"));
	if (TAR_HAS_OPT_LT(data.flags) && TAR_HAS_OPT_LX(data.flags))
		return (TAR_ERR("Can't specify both -t and -x\n"));
	tar = TAR_HAS_OPT_LC(data.flags);
	if (TAR_HAS_OPT_LF(data.flags))
	{
		if (!(fp = fopen((data.argv++)[0], (tar) ? "w" : "r")))
			return (TAR_ERR("%s", strerror(errno)));
	}
	else if (!(fp = fdopen((tar) ? 1 : 0, (tar) ? "w" : "r")))
		return (TAR_ERR("%s", strerror(errno)));
	return ((tar) ? ft_tar(data.argv, fp) : ft_untar(fp));
}
