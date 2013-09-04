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

#include <gio/gio.h>

#include "config.h"

/* ------------------------------------------------------------------- */

#define	GRP_GLOBAL	"global"

struct Config
{
	GKeyFile	*keyfile;
};

/* ------------------------------------------------------------------- */

static void config_set_defaults(GKeyFile *kf)
{
	g_key_file_set_boolean(kf, GRP_GLOBAL, "autodetect-on-startup", FALSE);
	g_key_file_set_boolean(kf, GRP_GLOBAL, "connect-all-on-first-autodetect", FALSE);
}

static bool get_filename(bool with_file, char *buf, size_t buf_max)
{
	const char *prg = "freemon";

	/* FIXME(emil): Come on, refactor this! */
	if(!with_file)
		return g_snprintf(buf, buf_max, "%s/%s", g_get_user_config_dir(), prg);
	return g_snprintf(buf, buf_max, "%s/%s/%s.conf", g_get_user_config_dir(), prg, prg) < buf_max;
}

static GKeyFile * config_load(void)
{
	char filename[1024];

	if(!get_filename(true, filename, sizeof filename))
		return false;

	printf("loading config from '%s'\n", filename);
	GKeyFile *kf = g_key_file_new();

	if(g_key_file_load_from_file(kf, filename, G_KEY_FILE_KEEP_COMMENTS, NULL))
	{
		printf("load OK\n");
	}
	else
	{
		g_key_file_free(kf);
		kf = NULL;
	}
	return kf;
}

static bool config_save(const Config *cfg)
{
	char filename[1024];
	if(!get_filename(true, filename, sizeof filename))
		return false;

	bool ok = false;
	gsize length;
	gchar *data = g_key_file_to_data(cfg->keyfile, &length, NULL);
	if(data != NULL)
	{
		char dirname[1024];

		if(get_filename(false, dirname, sizeof dirname))
			g_mkdir_with_parents(dirname, 0755);

		GFile *file = g_file_new_for_path(filename);
		GError *error = NULL;
		ok = g_file_replace_contents(file, data, length, NULL, FALSE, G_FILE_CREATE_NONE, NULL, NULL, &error);
		g_object_unref(G_OBJECT(file));
	}
	g_free(data);

	return ok;
}

Config * config_init(void)
{
	Config *cfg = g_malloc(sizeof *cfg);

	if((cfg->keyfile = config_load()) == NULL)
	{
		cfg->keyfile = g_key_file_new();
		config_set_defaults(cfg->keyfile);
		config_save(cfg);
	}
	return cfg;
}

bool config_get_refresh_on_startup(const Config *cfg)
{
	return false;
}
