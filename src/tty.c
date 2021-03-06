/*
 * Freemon: TTY (serial port) module.
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

#include <string.h>

#define	 __USE_BSD	/* For cfmakeraw(). */
#include <termios.h>
#include <unistd.h>

#include <glib-unix.h>

#include <gdk/gdk.h>

#include "tty.h"

/* ------------------------------------------------------------------- */

struct TtyInfo {
	const gchar	*device;
	guint		handle;
	unsigned int	baud_rate;
	int		fd;
};

/* ------------------------------------------------------------------- */

static tcflag_t serial_get_rate(unsigned int baud_rate)
{
	static const struct {
		unsigned int	rate;
		tcflag_t	flag;
	} rates[] = {
#define	DECL_RATE(r)	{ r, B ## r }
		DECL_RATE(50), DECL_RATE(75), DECL_RATE(110), DECL_RATE(134), DECL_RATE(150),
		DECL_RATE(200), DECL_RATE(300), DECL_RATE(600), DECL_RATE(1200), DECL_RATE(1800),
		DECL_RATE(2400), DECL_RATE(4800), DECL_RATE(9600), DECL_RATE(19200), DECL_RATE(38400),
		DECL_RATE(57600), DECL_RATE(115200), DECL_RATE(230400)
	};
	for(size_t i = 0; i < sizeof rates / sizeof *rates; ++i)
	{
		if(rates[i].rate == baud_rate)
			return rates[i].flag;
	}
	return B0;
}

/* Configure serial port for talking to the FRDM board; 115200 bps 8N1. */
static bool serial_configure(int fd)
{
	struct termios	newios;

	memset(&newios, 0, sizeof newios);
	cfmakeraw(&newios);
	newios.c_cflag |= serial_get_rate(115200);
	tcflush(fd, TCIOFLUSH);

	return tcsetattr(fd, TCSANOW, &newios) == 0;
}

/* ------------------------------------------------------------------- */

static gboolean cb_serial(gint fd, GIOCondition condition, gpointer user)
{
	char buf[4096];
	const ssize_t got = read(fd, buf, sizeof buf);
	if(got > 0)
	{
		target_gui_terminal_insert(user, buf, got);
	}
	return TRUE;
}

static void cb_keypress(guint32 unicode, gpointer user)
{
	char	out[1] = { unicode & 0xff };

	write(((TtyInfo *) user)->fd, out, 1);
}

TtyInfo * tty_open(const char *device, Target *target)
{
	TtyInfo	*serial;

	serial = g_malloc(sizeof *serial);
	errno = 0;
	serial->fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(serial->fd >= 0)
	{
		/* Configure serial port. */
		serial_configure(serial->fd);

		serial->handle = g_unix_fd_add(serial->fd, G_IO_IN, cb_serial, target);
		if(serial->handle != 0)
		{
			target_gui_terminal_set_keyhandler(target, cb_keypress, serial);
			return serial;
		}
		close(serial->fd);
	}
	else
	{
		const char *err = strerror(errno);
		fprintf(stderr, "Freemon: failed to open serial port '%s' (%s)\n", device, err);
	}
	g_free(serial);

	return NULL;
}

const char * tty_get_device(const TtyInfo *tty)
{
	return tty != NULL ? tty->device : NULL;
}

void tty_close(TtyInfo *tty)
{
	g_source_remove(tty->handle);
	close(((TtyInfo *) tty)->fd);
	g_free(tty);
}
