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

#include <stdio.h>

#include <gio/gio.h>

#include "boardid.h"

/* ------------------------------------------------------------------- */

bool boardid_identify(BoardId *id, const char *path)
{
	gchar fn[1024];

	if(g_snprintf(fn, sizeof fn, "%s" G_DIR_SEPARATOR_S "SDA_INFO.HTM", path) < sizeof fn)
	{
		GFile *file;

		file = g_file_new_for_path(fn);
		char *data;
		gsize length;
		if(g_file_load_contents(file, NULL, &data, &length, NULL, NULL))
		{
			printf("loaded: '%s'\n", data);
			g_free(data);
		}
		else
			printf("failed to open '%s'\n", fn);
		g_object_unref(file);
	}
	return false;
}

/* ------------------------------------------------------------------- */

bool boardid_equal(const BoardId *id1, const BoardId *id2)
{
	return false;
}
