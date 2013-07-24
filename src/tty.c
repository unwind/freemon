/*
 * frdm-mntr TTY module.
*/

#include "tty.h"

/* ------------------------------------------------------------------- */

TtyInfo * tty_open(Target *target, TtyType type, const char *device)
{
	TtyInfo	*tty;

	switch(type)
	{
	case TTY_TYPE_FIFO:
		break;
	case TTY_TYPE_SERIAL:
		tty = tty_serial_open(device, target);
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
