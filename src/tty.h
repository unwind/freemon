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

#if !defined TTY_H_
#define	TTY_H_

#include <stdbool.h>
#include <glib.h>

#include "target.h"

/* ------------------------------------------------------------------- */

typedef enum {
	TTY_TYPE_FIFO = 0,
	TTY_TYPE_SERIAL,
} TtyType;

/* Type-dependent structures embed this first. */
typedef struct {
	TtyType		type;
	const gchar	*device;
	guint		handle;
} TtyInfo;

#include "tty-fifo.h"
#include "tty-serial.h"

/* ------------------------------------------------------------------- */

TtyInfo *	tty_open(Target *target, TtyType type, const char *device);
void		tty_close(TtyInfo *tty);

#endif		/* TTY_H_ */
