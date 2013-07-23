/*
 * frdm-mntr FIFO TTY module.
*/

#include "tty.h"

/* ------------------------------------------------------------------- */

TtyInfo * tty_fifo_open(GuiInfo *gui, const char *device)
{
	tty->fd = open(device, O_RDONLY | O_NONBLOCK);
	if(tty->fd >= 0)
	{
		tty->handle = g_unix_fd_add(tty->fd, G_IO_IN, cb_tty, gui->terminal);
		printf("added '%s' as input source\n", device);
	}
	else
		tty->handle = 0;
}

void tty_fifo_close(TtyInfo *tty)
{
	close(tty->fd);
}
