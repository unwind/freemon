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
#include <string.h>

#include <gio/gio.h>

#include "boardid.h"

/* ------------------------------------------------------------------- */

void boardid_init(BoardId *id)
{
	memset(id, 0, sizeof *id);
}

bool boardid_valid(const BoardId *id)
{
	static const BoardId zero = { { 0 } };

	return !boardid_equal(id, &zero);
}

bool boardid_equal(const BoardId *id1, const BoardId *id2)
{
	if(strcmp(id1->board, id2->board) != 0)
		return false;
	if(memcmp(id1->tuid, id2->tuid, sizeof id1->tuid) != 0)
		return false;
	return true;
}

/* ------------------------------------------------------------------- */

static bool parse_html_input_board(BoardId *bid, const char *input, const char *end)
{
	const char *value = strstr(input, "value=\"");
	if(value != NULL)
	{
		return sscanf(value + 7, "%15[^\"]", bid->board) == 1;
	}
	return false;
}

static bool parse_html_input_tuid(BoardId *bid, const char *input, const char *end)
{
	const char *value = strstr(input, "value=\"");
	if(value != NULL)
	{
		unsigned int tmp[4];	/* Don't assume "unsigned int" is 32-bit. */
		if(sscanf(value + 7, "%x-%x-%x-%x", tmp, tmp + 1, tmp + 2, tmp + 3) == 4)
		{
			/* This makes sure we never partially change tuid. */
			for(int i = 0; i < 4; ++i)
				bid->tuid[i] = tmp[i];
			return true;
		}
	}
	return false;
}

static const char * parse_html_input(BoardId *bid, const char *input, const char *end)
{
	const char *board = strstr(input, "id=\"BOARD");
	if(board != NULL)
		parse_html_input_board(bid, input, end);
	const char *tuid = strstr(input, "id=\"TUID");
	if(tuid != NULL)
		parse_html_input_tuid(bid, input, end);
	return end + 1;
}

bool boardid_set_from_target(BoardId *id, const char *path)
{
	gchar fn[1024];
	if(g_snprintf(fn, sizeof fn, "%s" G_DIR_SEPARATOR_S "SDA_INFO.HTM", path) < sizeof fn)
	{
		GFile *file = g_file_new_for_path(fn);
		char *data;
		gsize length;
		if(g_file_load_contents(file, NULL, &data, &length, NULL, NULL))
		{
			const char *iter = data, *input;

			boardid_init(id);
			while((input = strstr(iter, "<input")) != NULL)
			{
				char *end = strstr(input, "/>");
				if(!end)
					break;
				*end = '\0';	/* This is fine, we own the memory. */
				iter = parse_html_input(id, input, end);
			}
			g_free(data);
		}
		else
			fprintf(stderr, "**Failed to open '%s'\n", fn);
		g_object_unref(file);
	}
	return false;
}
