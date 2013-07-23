/*
 * frdm-mntr FIFO TTY module.
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
		gui_terminal_insert(user, buf, got);
	}
	return TRUE;
}

TtyInfo * tty_fifo_open(GuiInfo *gui, const char *device)
{
	TtyInfoFifo	*fifo;

	fifo = g_malloc(sizeof *fifo);
	fifo->fd = open(device, O_RDONLY | O_NONBLOCK);
	if(fifo->fd >= 0)
	{
		fifo->tty.handle = g_unix_fd_add(fifo->fd, G_IO_IN, cb_fifo, gui);
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
