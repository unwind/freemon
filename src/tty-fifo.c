/*
 * Freemon: TTY FIFO module.
 *
 * Copyright 2013 Emil Brink <emil@obsession.se>.
 * 
 * This file is part of Freemon.
 *
 * Freemon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Freemon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Freemon.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <glib-unix.h>

#include "tty.h"

typedef struct {
	TtyInfo	tty;
	int	fd;
} TtyInfoFifo;

/* ------------------------------------------------------------------- */

static gboolean cb_fifo(gint fd, GIOCondition condition, gpointer user)
{
	char	buf[4096];

	ssize_t	got = read(fd, buf, sizeof buf);
	if(got > 0)
	{
		target_gui_terminal_insert(user, buf, got);
	}
	return TRUE;
}

TtyInfo * tty_fifo_open(const char *device, Target *target)
{
	TtyInfoFifo	*fifo;

	fifo = g_malloc(sizeof *fifo);
	fifo->fd = open(device, O_RDONLY | O_NONBLOCK);
	if(fifo->fd >= 0)
	{
		fifo->tty.handle = g_unix_fd_add(fifo->fd, G_IO_IN, cb_fifo, target);
		if(fifo->tty.handle != 0)
		{
			printf("added '%s' as input source\n", device);
			return &fifo->tty;	/* Avoids cast. :) */
		}
		close(fifo->fd);
	}
	g_free(fifo);

	return NULL;
}

void tty_fifo_close(TtyInfo *tty)
{
	g_source_remove(tty->handle);
	close(((TtyInfoFifo *) tty)->fd);
	g_free(tty);
}
