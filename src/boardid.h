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
	char		board[16];	/* A name, such as "FRDM-KL25Z". */
	uint32_t	tuid[4];
} BoardId;

/* ------------------------------------------------------------------- */

void		boardid_init(BoardId *id);
bool		boardid_valid(const BoardId *id);
guint		boardid_hash(gconstpointer key);
gboolean	boardid_equal(gconstpointer a, gconstpointer b);

bool		boardid_set_from_target(BoardId *id, const char *path);

bool		boardid_to_keyfile_group(const BoardId *id, char *group, size_t group_max);
bool		boardid_from_keyfile_group(BoardId *id, const char *group);

#endif		/* BOARDID_H_ */
