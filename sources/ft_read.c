/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_read.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcarra <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/08 09:33:43 by jcarra            #+#    #+#             */
/*   Updated: 2017/02/08 09:33:44 by jcarra           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "shell.h"
#include "terms.h"

size_t		g_position;
size_t		nb;
char		**g_line;
char		**env;

void		ft_sigint(int sig)
{
	if (sig != SIGINT)
		return ;
	ft_putendl_fd("", 0);
	ft_affprompt(nb, env);
	if (g_line)
	{
		free(*g_line);
		*g_line = ft_strnew(1);
		g_position = 0;
	}
}

static void	ft_read_history_do(char **str, t_sys **sys, size_t *i, size_t *pos)
{
	t_lst	*tmp;

	if (*i >= (size_t)ft_list_size((*sys)->history))
		return ;
	*i = *i + 1;
	ft_read_move(&(*str), KEY_HOM, &(*pos));
	ft_read_history_clear(*str);
	tmp = ft_list_at((*sys)->history, *i);
	free(*str);
	if (tmp)
	{
		if ((*str = ft_strdup(tmp->data)) != NULL)
			ft_putstr_fd(*str, 0);
	}
	else
		*str = ft_strnew(1);
	*pos = (tmp && *str) ? ft_strlen(*str) : 0;
}

static void	ft_read_history_up(char **str, t_sys **sys, size_t *i, size_t *pos)
{
	t_lst	*tmp;
	char	ret;

	if (*i == 0)
		return ;
	if (*i == (size_t)ft_list_size((*sys)->history))
	{
		ret = ft_history_maj(&(*sys)->history, ft_strdup(*str), (*sys)->env);
		*i = (size_t)ft_list_size((*sys)->history) - ((ret) ? 1 : 0);
	}
	*i = *i - 1;
	ft_read_move(&(*str), KEY_HOM, &(*pos));
	ft_read_history_clear(*str);
	tmp = ft_list_at((*sys)->history, *i);
	if (tmp)
	{
		free(*str);
		if ((*str = ft_strdup(tmp->data)) != NULL)
			ft_putstr_fd(*str, 0);
	}
	else
		*i = *i + 1;
    *pos = (tmp && *str) ? ft_strlen(*str) : 0;
}

static char	ft_read_check(int c, char **str, t_sys **sys, size_t *n)
{
	if (c == KEY_RET)
		return (TRUE);
	if (ft_isprint(c) || c == KEY_TAB)
		ft_read_print(&(*str), c, &(*n), (*sys)->env);
	if (c == KEY_BAC)
		ft_read_delete(&(*str), &(*n));
	if (c == KEY_DEL)
		ft_read_suppr(&(*str), &(*n));
	if (c == KEY_HOM || c == KEY_ENS || c == KEY_LEF || c == KEY_RIG ||
		c == KEY_OLE || c == KEY_ORI)
		ft_read_move(&(*str), c, &(*n));
	return (FALSE);
}

int			ft_read(char **str, t_sys **sys, size_t n)
{
	size_t	i;
	char	exit;
	int		c;

	nb = n;
	env = (*sys)->env;
	g_position = 0;
	i = (size_t)ft_list_size((*sys)->history);
	exit = FALSE;
	*str = ft_strnew(1);
	g_line = &(*str);
	while (exit == FALSE)
	{
		c = 0;
		read(0, &c, sizeof(int));
		ft_removecompl(&(*str));
		exit = ft_read_check(c, &(*str), &(*sys), &g_position);
		if (c == KEY_EOF && !ft_strlen(*str))
			return (FALSE);
		if (c == KEY_UPS)
			ft_read_history_up(&(*str), &(*sys), &i, &g_position);
		if (c == KEY_DOW)
			ft_read_history_do(&(*str), &(*sys), &i, &g_position);
	}
	ft_print(*str, g_position, 0);
	ft_putchar_fd('\n', 0);
	return (TRUE);
}
