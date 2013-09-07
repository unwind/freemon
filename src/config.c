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
#include <gtk/gtk.h>

#include "config.h"

/* ------------------------------------------------------------------- */

#define	GRP_GLOBAL	"global"

typedef enum {
	SIMPLETYPE_END_OF_LIST,	/* Sentinel value. */
	SIMPLETYPE_GROUP,	/* This is not an actual setting, it's used as a grouping meta thingie. */
	SIMPLETYPE_BOOLEAN,
	SIMPLETYPE_INTEGER,
	SIMPLETYPE_STRING,
} SimpleType;

typedef union {
	bool boolean;
	int integer;
	const char *string;
} SimpleValue;

typedef struct {
	SimpleType	type;
	SimpleValue	value;
	const char	*key;
	const char	*comment;
} SettingTemplate;

static const SettingTemplate global_settings[] = {
	{ SIMPLETYPE_GROUP, .key = GRP_GLOBAL, .comment = "Global Settings" },
	{ SIMPLETYPE_BOOLEAN, .value.boolean = false, "autodetect-on-startup", "Automatically detect plugged-in boards on startup?" },
	{ SIMPLETYPE_BOOLEAN, .value.boolean = false, "connect-all-on-first-autodetect", "Connect to all plugged-in boards the first time autodetect is run?" },
	{ SIMPLETYPE_END_OF_LIST }
};

/* ------------------------------------------------------------------- */

struct Config
{
	GHashTable	*meta;		/* Maps setting keys back to SettingTemplates, for meta info. */
	GKeyFile	*keyfile;
};

/* ------------------------------------------------------------------- */

static void config_keyfile_add_from_templates(Config *cfg, const SettingTemplate *templates)
{
	GKeyFile *kf = cfg->keyfile;
	const char *group = NULL;

	while(true)
	{
		switch(templates->type)
		{
		case SIMPLETYPE_GROUP:
			group = templates->key;
			continue;	/* Avoid hashing the group. */
		case SIMPLETYPE_BOOLEAN:
			g_key_file_set_boolean(kf, group, templates->key, templates->value.boolean);
			break;
		case SIMPLETYPE_INTEGER:
			g_key_file_set_integer(kf, group, templates->key, templates->value.integer);
			break;
		case SIMPLETYPE_STRING:
			g_key_file_set_string(kf, group, templates->key, templates->value.string);
			break;
		default:
			return;
		}
		g_hash_table_insert(cfg->meta, (gpointer) templates->key, (gpointer) templates);
		++templates;
	}
}

static void config_keyfile_set_defaults(Config *cfg)
{
	config_keyfile_add_from_templates(cfg, global_settings);
}

/* Copy the settings into a newly created configurations empty keyfile. Also initializes the meta hashing. */
static void config_keyfile_copy_with_templates(Config *cfg, const Config *src, const SettingTemplate *templates)
{
	GKeyFile *kf_s = src->keyfile, *kf_d = cfg->keyfile;
	const char *group = NULL;
	gchar *s;

	while(templates->type != SIMPLETYPE_END_OF_LIST)
	{
		switch(templates->type)
		{
		case SIMPLETYPE_GROUP:
			group = templates->key;
			++templates;
			continue;	/* Avoid hashing the group. */
		case SIMPLETYPE_BOOLEAN:
			g_key_file_set_boolean(kf_d, group, templates->key, g_key_file_get_boolean(kf_s, group, templates->key, NULL));
			break;
		case SIMPLETYPE_INTEGER:
			g_key_file_set_integer(kf_d, group, templates->key, g_key_file_get_integer(kf_s, group, templates->key, NULL));
			break;
		case SIMPLETYPE_STRING:
			s = g_key_file_get_string(kf_s, group, templates->key, NULL);
			g_key_file_set_string(kf_d, group, templates->key, s != NULL ? s : "");
			g_free(s);
			break;
		default:
			return;
		}
		g_hash_table_insert(cfg->meta, (gpointer) templates->key, (gpointer) templates);
		++templates;
	}
}

static bool get_filename(bool with_file, char *buf, size_t buf_max)
{
	const char *prg = "freemon";
	const gchar *dir = g_get_user_config_dir();

	/* FIXME(emil): Come on, refactor this! */
	if(!with_file)
		return g_snprintf(buf, buf_max, "%s/%s", dir, prg);
	return g_snprintf(buf, buf_max, "%s/%s/%s.conf", dir, prg, prg) < buf_max;
}

static GKeyFile * config_keyfile_load(void)
{
	char filename[1024];

	if(!get_filename(true, filename, sizeof filename))
		return false;

	GKeyFile *kf = g_key_file_new();

	if(!g_key_file_load_from_file(kf, filename, G_KEY_FILE_KEEP_COMMENTS, NULL))
	{
		/* Load failed, free the empty key file. */
		g_key_file_free(kf);
		kf = NULL;
	}
	return kf;
}

static bool config_keyfile_save(const Config *cfg)
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

static Config * config_new(void)
{
	Config *cfg = g_malloc(sizeof *cfg);

	cfg->meta = g_hash_table_new(g_str_hash, g_str_equal);
	cfg->keyfile = NULL;

	return cfg;
}

Config * config_init(void)
{
	Config *cfg = config_new();
	if((cfg->keyfile = config_keyfile_load()) == NULL)
	{
		cfg->keyfile = g_key_file_new();

		/* After creating a new default config, save it. */
		config_keyfile_set_defaults(cfg);
		config_keyfile_save(cfg);
	}
	return cfg;
}

/* ------------------------------------------------------------------- */

Config * config_copy(const Config *cfg)
{
	if(cfg == NULL)
		return NULL;

	Config *copy = config_new();
	copy->keyfile = g_key_file_new();

	config_keyfile_copy_with_templates(copy, cfg, global_settings);

	return copy;
}

bool config_save(const Config *cfg)
{
	return config_keyfile_save(cfg);
}

void config_delete(Config *cfg)
{
	if(cfg != NULL)
	{
		g_key_file_free(cfg->keyfile);
		g_hash_table_destroy(cfg->meta);
		g_free(cfg);
	}
}

/* ------------------------------------------------------------------- */

static void widget_data_set(GtkWidget *wid, const char *group, const SettingTemplate *template)
{
	g_object_set_data(G_OBJECT(wid), "group", (gpointer) group);
	g_object_set_data(G_OBJECT(wid), "template", (gpointer) template);
}

static void widget_data_get(GtkWidget *wid, const char **group, const SettingTemplate **template)
{
	*group = g_object_get_data(G_OBJECT(wid), "group");
	*template = g_object_get_data(G_OBJECT(wid), "template");
}

static void evt_check_button_toggled(GtkWidget *wid, gpointer user)
{
	const char *group;
	const SettingTemplate *template;
	Config *cfg = user;

	widget_data_get(wid, &group, &template);
	const gboolean value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wid));
	g_key_file_set_boolean(cfg->keyfile, group, template->key, value);
}

static void init_check_button(GtkWidget *wid, Config *cfg, const char *group, const SettingTemplate *template)
{
	widget_data_set(wid, group, template);
	const gboolean value = g_key_file_get_boolean(cfg->keyfile, group, template->key, NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wid), value);
	g_signal_connect(G_OBJECT(wid), "toggled", G_CALLBACK(evt_check_button_toggled), cfg);
}

static GtkWidget * build_editor_from_templates(Config *cfg, const SettingTemplate *templates)
{
	GtkWidget *frame = NULL, *grid = NULL, *wid;
	const char *group = NULL;
	int y = 0;

	while(templates->type != SIMPLETYPE_END_OF_LIST)
	{
		switch(templates->type)
		{
		case SIMPLETYPE_GROUP:
			group = templates->key;
			frame = gtk_frame_new(templates->comment);
			grid = gtk_grid_new();
			gtk_container_add(GTK_CONTAINER(frame), grid);
			break;
		case SIMPLETYPE_BOOLEAN:
			wid = gtk_check_button_new_with_label(templates->comment);
			init_check_button(wid, cfg, group, templates);
			gtk_grid_attach(GTK_GRID(grid), wid, 0, y, 1, 1);
			++y;
			break;
		default:
			break;
		}
		++templates;
	}
	return frame;
}

Config * config_edit(const Config *cfg, GtkWindow *parent)
{
	Config *editing = config_copy(cfg);

	GtkWidget *dlg = gtk_dialog_new_with_buttons("Preferences", parent, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

	GtkWidget *global = build_editor_from_templates(editing, global_settings);
	gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dlg))), global);

	gtk_widget_show_all(dlg);
	const gint response = gtk_dialog_run(GTK_DIALOG(dlg));
	gtk_widget_destroy(dlg);

	if(response != GTK_RESPONSE_OK)
	{
		config_delete(editing);
		editing = NULL;
	}
	return editing;
}
