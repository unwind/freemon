/*
 * Freemon: target autodetection module.
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

#if !defined AUTODETECT_H_
#define	AUTODETECT_H_

#include <stdbool.h>
#include <gtk/gtk.h>

#include "boardid.h"

/* ------------------------------------------------------------------- */

typedef struct {
	char	device[32];	/* Serial port device (for terminal). */
	char	path[64];	/* Mountpoint (for upload). */
	BoardId	id;
	char	name[32];	/* Initialized by the config module. */
} AutodetectedTarget;

/* ------------------------------------------------------------------- */

GSList *	autodetect_all(void);

void		autodetect_target_set_name(AutodetectedTarget *at, const char *name);

bool		autodetect_target_to_string(const AutodetectedTarget *at, char *buf, size_t buf_max);

void		autodetect_free(GSList *list);

#endif		/* AUTODETECT_H_ */
