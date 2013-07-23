/*
 * frdm-mntr TTY module.
*/

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

TtyInfo * tty_open(GuiInfo *gui, TtyType type, const char *device)
{
	TtyInfo	*tty;

	switch(type)
	{
	case TTY_TYPE_FIFO:
		break;
	case TTY_TYPE_SERIAL:
		tty = tty_serial_open(gui, device);
		break;
	default:
		return NULL;
	}

	if(tty != NULL)
	{
		tty->type = type;
		tty->device = device;
	}
	return tty;
}

void tty_close(TtyInfo *tty)
{
	if(tty != NULL)
	{
		switch(tty->type)
		{
		case TTY_TYPE_FIFO:
			break;
		case TTY_TYPE_SERIAL:
			tty_serial_close(tty);
			break;
		}
	}
}
