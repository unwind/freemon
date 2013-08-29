/*
 * Freemon: board identification module.
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

#if !defined BOARDID_H_
#define	BOARDID_H_

#include <stdbool.h>
#include <stdint.h>

/* ------------------------------------------------------------------- */

typedef struct {
	uint32_t	tuid[4];
} BoardId;

/* ------------------------------------------------------------------- */

bool	boardid_identify(BoardId *id, const char *path);
bool	boardid_equal(const BoardId *id1, const BoardId *id2);

#endif		/* BOARDID_H_ */
