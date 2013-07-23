/*
 * frdm-mntr serial port TTY module.
*/

#include <string.h>

#define	 __USE_BSD	/* For cfmakeraw(). */
#include <termios.h>
#include <unistd.h>

#include <glib-unix.h>

#include "tty.h"

/* ------------------------------------------------------------------- */

typedef struct {
	TtyInfo		tty;
	unsigned int	baud_rate;
	int		fd;
} TtyInfoSerial;

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
	char	buf[4096];

	ssize_t	got = read(fd, buf, sizeof buf);
	if(got > 0)
	{
		gui_terminal_insert(user, buf, got);
	}
	return TRUE;
}

TtyInfo * tty_serial_open(GuiInfo *gui, const char *device)
{
	TtyInfoSerial	*serial;

	serial = g_malloc(sizeof *serial);
	serial->fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(serial->fd >= 0)
	{
		/* Configure serial port. */
		serial_configure(serial->fd);

		serial->tty.handle = g_unix_fd_add(serial->fd, G_IO_IN, cb_serial, gui);
		if(serial->tty.handle != 0)
		{
			printf("added '%s' as input source\n", device);
			return &serial->tty;	/* Avoids cast. :) */
		}
		close(serial->fd);
	}
	g_free(serial);

	return NULL;
}

void tty_serial_close(TtyInfo *tty)
{
	g_source_remove(tty->handle);
	close(((TtyInfoSerial *) tty)->fd);
	g_free(tty);
}
