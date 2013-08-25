/*
 * Freemon: TTY module.
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
