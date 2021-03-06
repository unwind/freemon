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

guint boardid_hash(gconstpointer key)
{
	const BoardId *id = key;
	const guint h1 = g_int_hash(&id->tuid[0]);
	const guint h2 = g_int_hash(&id->tuid[1]);
	const guint h3 = g_int_hash(&id->tuid[2]);
	const guint h4 = g_int_hash(&id->tuid[3]);

	return h1 ^ h2 ^ h3 ^ h4;
}

gboolean boardid_equal(gconstpointer a, gconstpointer b)
{
	const BoardId *id1 = a, *id2 = b;
	if(strcmp(id1->board, id2->board) != 0)
		return FALSE;
	if(memcmp(id1->tuid, id2->tuid, sizeof id1->tuid) != 0)
		return FALSE;
	return TRUE;
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
	boardid_init(id);
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
	return boardid_valid(id);
}

/* ------------------------------------------------------------------- */

bool boardid_to_keyfile_group(const BoardId *id, char *group, size_t group_max)
{
	return g_snprintf(group, group_max, "board(%s,%08x.%08x.%08x.%08x)",
		id->board,
		id->tuid[0], id->tuid[1], id->tuid[2], id->tuid[3]) < group_max;
}

bool boardid_from_keyfile_group(BoardId *id, const char *group)
{
	if(strncmp(group, "board(", 6) == 0)
	{
		const char *board = group + 6;
		const char *comma = strrchr(group, ',');
		if(comma != NULL)
		{
			const size_t board_len = comma - board;
			if(board_len <= (sizeof id->board - 1))
			{
				memcpy(id->board, board, board_len);
				id->board[board_len] = '\0';
				if(sscanf(comma + 1, "%x.%x.%x.%x", &id->tuid[0], &id->tuid[1], &id->tuid[2], &id->tuid[3]) == 4)
					return true;
			}
		}
	}
	return false;
}
