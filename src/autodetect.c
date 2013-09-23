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

#if defined __linux
#include "autodetect-linux.c"
#else
#error "Missing autodetect implementation for this platform, sorry!"
#endif

/* ------------------------------------------------------------------- */

void autodetect_target_set_name(AutodetectedTarget *at, const char *name)
{
	g_strlcpy(at->name, name, sizeof at->name);
}

bool autodetect_target_to_string(const AutodetectedTarget *at, char *buf, size_t buf_max)
{
	if(at->name[0] != '\0')
		return g_snprintf(buf, buf_max, "%s [%s on %s]", at->name, at->id.board, at->device) < buf_max;
	return g_snprintf(buf, buf_max, "%s on %s", at->id.board, at->device) < buf_max;
}

/* ------------------------------------------------------------------- */

void autodetect_free(GSList *list)
{
	for(GSList *iter = list; iter != NULL; iter = g_slist_next(iter))
		g_free(iter->data);
	g_slist_free(list);
}
