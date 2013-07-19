/*
 * frdm-mntr TTY module.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <glib-unix.h>

#include "tty.h"

/* ------------------------------------------------------------------- */

static gboolean cb_tty(gint fd, GIOCondition cond, gpointer user)
{
	if(cond != G_IO_IN)
		return FALSE;

	printf("hello from tty device\n");

	gchar buf[4096];
	const ssize_t got = read(fd, buf, sizeof buf);
	if(got > 0)
	{
		printf("got %zd bytes\n", got);
	}
	return TRUE;
}

/* ------------------------------------------------------------------- */

bool tty_open(TtyInfo *tty, GuiInfo *gui, const char *device)
{
	tty->device = device;	/* FIXME: Assume, assume. */
	tty->fd = open(device, O_RDONLY);

	if(tty->fd >= 0)
	{
		tty->handler = g_unix_fd_add(tty->fd, G_IO_IN, cb_tty, gui->terminal);
		printf("added '%s' as input source\n", device);
	}
	else
		tty->handler = 0;

	return tty->handler != 0;
}

void tty_close(TtyInfo *tty)
{
	close(tty->fd);
}
