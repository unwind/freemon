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

	gchar buf[4096];	/* This should be plenty, for the real device. */
	const ssize_t got = read(fd, buf, sizeof buf);
	if(got > 0)
	{
		printf("got %zd bytes\n", got);
		vte_terminal_feed(VTE_TERMINAL(user), buf, got);
	}
	return TRUE;
}

/* ------------------------------------------------------------------- */

bool tty_open(TtyInfo *tty, GuiInfo *gui, const char *device)
{
	tty->device = device;	/* FIXME: Assume, assume. */

	tty->fd = open(device, O_RDONLY | O_NONBLOCK);
	if(tty->fd >= 0)
	{
		tty->handle = g_unix_fd_add(tty->fd, G_IO_IN, cb_tty, gui->terminal);
		printf("added '%s' as input source\n", device);
	}
	else
		tty->handle = 0;

	return tty->handle != 0;
}

void tty_close(TtyInfo *tty)
{
	g_source_remove(tty->handle);
	close(tty->fd);
}
