/*
 * Freemon: persistent configuration module.
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

#include <stdbool.h>
#include <stdio.h>

#include <glib.h>

#include "config.h"

/* ------------------------------------------------------------------- */

typedef enum {
	VALUE_TYPE_BOOLEAN = 0,
} ValueType;

typedef struct {
	ValueType	type;
	union {
	bool		boolean;
	}		u;
} Value;

typedef struct {
	const char	*symbol;
	Value		value;
	const char	*label;
} Setting;

typedef struct BoardConfig
{
	Setting	auto_upload;
	Setting	upload_resets_tty;
} BoardConfig;

struct Config
{
	Setting	refresh_on_startup;
	GSList	*board_configs;
};

/* ------------------------------------------------------------------- */

static bool get_filename(char *buf, size_t buf_max)
{
	const char *prg = "freemon";

	return g_snprintf(buf, buf_max, "%s/%s/%s.conf", g_get_user_config_dir(), prg, prg) < buf_max;
}

static bool config_load(Config *cfg)
{
	char buf[1024];

	if(!get_filename(buf, sizeof buf))
		return false;

	GKeyFile *kf = g_key_file_new();
	if(g_key_file_load_from_file(kf, buf, G_KEY_FILE_KEEP_COMMENTS, NULL))
	{
	}
	g_key_file_free(kf);
}

Config * config_init(void)
{
	return NULL;
}

bool config_get_refresh_on_startup(const Config *cfg)
{
	return cfg != NULL ? cfg->refresh_on_startup.value.u.boolean : false;
}
