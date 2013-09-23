/*
 * Freemon: TTY serial port module.
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

#if !defined TTY_SERIAL_H_
#define	TTY_SERIAL_H_

#include "target.h"

/* ------------------------------------------------------------------- */

typedef struct TtyInfo	TtyInfo;

TtyInfo *	tty_serial_open(const char *device, Target *target);
const char *	tty_serial_get_device(const TtyInfo *tty);
void		tty_serial_close(TtyInfo *tty);

#endif		/* TTY_SERIAL_H_ */
