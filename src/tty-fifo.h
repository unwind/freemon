/*
 * Freemon: TTY FIFO module. Never include this directly, use "tty.h".
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

#if !defined TTY_FIFO_H_
#define	TTY_FIFO_H_

/* ------------------------------------------------------------------- */

TtyInfo *	tty_fifo_open(const char *device, Target *target);
void		tty_fifo_close(TtyInfo *tty);

#endif		/* TTY_FIFO_H_ */
