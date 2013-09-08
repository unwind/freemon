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

#include "autodetect.h"
#include "config.h"

/* ------------------------------------------------------------------- */

#define	GRP_GLOBAL	"global"

typedef enum {
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
	SimpleValue	default_value;
	const char	*key;
	const char	*comment;
} SettingTemplate;

#define	TMPL_REF(k)			setting_ ## k
#define	TMPL_DEF_GROUP(k, c)		static const SettingTemplate TMPL_REF(k) = { .type = SIMPLETYPE_GROUP, .key = #k, .comment = c }
#define	TMPL_DEF_BOOLEAN(k, v, c)	static const SettingTemplate TMPL_REF(k) = { .type = SIMPLETYPE_BOOLEAN, .default_value.boolean = v, .key = #k, .comment = c }
#define	TMPL_DEF_STRING(k, v, c)	static const SettingTemplate TMPL_REF(k) = { .type = SIMPLETYPE_STRING, .default_value.string = v, .key = #k, .comment = c }
#define TMPL_KEY(k)			TMPL_REF(k).key

TMPL_DEF_GROUP(global, "Global settings");
TMPL_DEF_BOOLEAN(autodetect_on_startup, false, "Automatically detect plugged-in boards on startup?");
TMPL_DEF_BOOLEAN(connect_first_on_first_autodetect, false, "Connect to first plugged-in board, the first time autodetect is run?");

static const SettingTemplate *global_settings[] = {
	&TMPL_REF(global),
	&TMPL_REF(autodetect_on_startup),
	&TMPL_REF(connect_first_on_first_autodetect),
	NULL
};

TMPL_DEF_GROUP(board, "Per-board settings");
TMPL_DEF_STRING(board_name, "", "User-friendly name for this board");
TMPL_DEF_BOOLEAN(reset_tty_on_upload, false, "Reset (clear) the terminal window after uploading new code to the board?");
TMPL_DEF_BOOLEAN(upload_on_change, false, "Monitor the selected binary for changes, and upload new files automatically?");

static const SettingTemplate *board_settings[] = {
	&TMPL_REF(board_name),
	&TMPL_REF(reset_tty_on_upload),
	&TMPL_REF(upload_on_change),
	NULL
};

/* ------------------------------------------------------------------- */

typedef struct {
	BoardId	id;
	char	group[64];
} KnownBoard;

struct Config
{
	GKeyFile	*keyfile;
	GHashTable	*known_boards;	/* Hashing BoardIds to KnownBoard instances. */
};

/* ------------------------------------------------------------------- */

static const char * keyfile_add_from_template(GKeyFile *kf, const char *group, const SettingTemplate *template)
{
	switch(template->type)
	{
	case SIMPLETYPE_GROUP:
		return template->key;
	case SIMPLETYPE_BOOLEAN:
		g_key_file_set_boolean(kf, group, template->key, template->default_value.boolean);
		break;
	case SIMPLETYPE_INTEGER:
		g_key_file_set_integer(kf, group, template->key, template->default_value.integer);
		break;
	case SIMPLETYPE_STRING:
		g_key_file_set_string(kf, group, template->key, template->default_value.string);
		break;
	}
	return group;
}

static void config_keyfile_add_from_templates(Config *cfg, const SettingTemplate **templates)
{
	GKeyFile *kf = cfg->keyfile;
	const char *group = NULL;

	for(const SettingTemplate *here = *templates; here != NULL; here = *++templates)
	{
		group = keyfile_add_from_template(kf, group, here);
	}
}

static void config_keyfile_set_defaults(Config *cfg)
{
	config_keyfile_add_from_templates(cfg, global_settings);
}

/* Copy the settings into a newly created configurations empty keyfile. Also initializes the meta hashing. */
static void config_keyfile_copy_with_templates(Config *cfg, const Config *src, const SettingTemplate **templates)
{
	GKeyFile *kf_s = src->keyfile, *kf_d = cfg->keyfile;
	const char *group = NULL;
	gchar *s;

	for(const SettingTemplate *here = *templates; here != NULL; here = *++templates)
	{
		switch(here->type)
		{
		case SIMPLETYPE_GROUP:
			group = here->key;
			continue;	/* Avoid hashing the group. */
		case SIMPLETYPE_BOOLEAN:
			g_key_file_set_boolean(kf_d, group, here->key, g_key_file_get_boolean(kf_s, group, here->key, NULL));
			break;
		case SIMPLETYPE_INTEGER:
			g_key_file_set_integer(kf_d, group, here->key, g_key_file_get_integer(kf_s, group, here->key, NULL));
			break;
		case SIMPLETYPE_STRING:
			s = g_key_file_get_string(kf_s, group, here->key, NULL);
			g_key_file_set_string(kf_d, group, here->key, s != NULL ? s : "");
			g_free(s);
			break;
		default:
			break;
		}
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

	cfg->keyfile = NULL;
	cfg->known_boards = g_hash_table_new(boardid_hash, boardid_equal);

	return cfg;
}

/* Initialize hash of known boards by iterating over board-sounding groups in the keyfile. */
static void config_init_boards_from_keyfile(Config *cfg)
{
	gchar **groups = g_key_file_get_groups(cfg->keyfile, NULL);
	for(int i = 0; groups[i] != NULL; ++i)
	{
		BoardId id;

		if(boardid_from_keyfile_group(&id, groups[i]))
		{
			KnownBoard *kb = g_malloc(sizeof *kb);
			kb->id = id;
			g_hash_table_insert(cfg->known_boards, &kb->id, kb);
		}
	}
	g_strfreev(groups);
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
	else
		config_init_boards_from_keyfile(cfg);
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
		g_free(cfg);
	}
}

/* ------------------------------------------------------------------- */

static void object_data_set(GObject *obj, const char *group, const SettingTemplate *template)
{
	g_object_set_data(obj, "group", (gpointer) group);
	g_object_set_data(obj, "template", (gpointer) template);
}

static void object_data_get(GObject *obj, const char **group, const SettingTemplate **template)
{
	*group = g_object_get_data(obj, "group");
	*template = g_object_get_data(obj, "template");
}

static void evt_check_button_toggled(GtkWidget *wid, gpointer user)
{
	const char *group;
	const SettingTemplate *template;
	Config *cfg = user;

	object_data_get(G_OBJECT(wid), &group, &template);
	const gboolean value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wid));
	g_key_file_set_boolean(cfg->keyfile, group, template->key, value);
}

static void init_check_button(GtkWidget *wid, Config *cfg, const char *group, const SettingTemplate *template)
{
	object_data_set(G_OBJECT(wid), group, template);
	const gboolean value = g_key_file_get_boolean(cfg->keyfile, group, template->key, NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wid), value);
	g_signal_connect(G_OBJECT(wid), "toggled", G_CALLBACK(evt_check_button_toggled), cfg);
}

static void evt_string_edited(GObject *obj, Config *cfg)
{
	const char *group;
	const SettingTemplate *template;

	object_data_get(obj, &group, &template);
	const gchar *text = gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(obj));
	g_key_file_set_string(cfg->keyfile, group, template->key, text);
}

static void evt_string_inserted_text(GObject *obj, guint position, gchar *chars, guint n_chars, gpointer user)
{
	evt_string_edited(obj, user);
}

static void evt_string_deleted_text(GObject *obj, guint position, guint n_chars, gpointer user)
{
	evt_string_edited(obj, user);
}

static void init_entry(GtkWidget *wid, Config *cfg, const char *group, const SettingTemplate *template)
{
	gchar *s = g_key_file_get_string(cfg->keyfile, group, template->key, NULL);
	gtk_entry_set_text(GTK_ENTRY(wid), s);
	g_free(s);
	GtkEntryBuffer *buf = gtk_entry_get_buffer(GTK_ENTRY(wid));
	object_data_set(G_OBJECT(buf), group, template);
	g_signal_connect(G_OBJECT(buf), "inserted_text", G_CALLBACK(evt_string_inserted_text), cfg);
	g_signal_connect(G_OBJECT(buf), "deleted_text", G_CALLBACK(evt_string_deleted_text), cfg);
}

static GtkWidget * build_editor_from_templates(Config *cfg, const char *group, const char *group_comment, const SettingTemplate **templates)
{
	GtkWidget *frame = NULL, *grid = NULL, *wid;
	int y = 0;

	for(const SettingTemplate *here = *templates; here != NULL; here = *++templates)
	{
		if(group != NULL && frame == NULL)
		{
			frame = gtk_frame_new(group_comment);
			grid = gtk_grid_new();
			gtk_container_add(GTK_CONTAINER(frame), grid);
		}

		wid = NULL;
		switch(here->type)
		{
		case SIMPLETYPE_GROUP:
			group = here->key;
			group_comment = here->comment;
			break;
		case SIMPLETYPE_BOOLEAN:
			wid = gtk_check_button_new_with_label(here->comment);
			init_check_button(wid, cfg, group, here);
			break;
		case SIMPLETYPE_STRING:
			{
				wid = gtk_grid_new();
				GtkWidget *label = gtk_label_new(here->comment);
				gtk_grid_attach(GTK_GRID(wid), label, 0, 0, 1, 1);
				GtkWidget *entry = gtk_entry_new();
				init_entry(entry, cfg, group, here);
				gtk_widget_set_hexpand(entry, TRUE);
				gtk_grid_attach(GTK_GRID(wid), entry, 1, 0, 1, 1);
			}
			break;
		default:
			break;
		}
		if(wid != NULL)
		{
			gtk_grid_attach(GTK_GRID(grid), wid, 0, y, 1, 1);
			++y;
		}
	}
	return frame;
}

static void evt_boards_row_activated(GtkWidget *wid, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user)
{
	Config *cfg = user;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(wid));
	GtkTreeIter iter;

	if(gtk_tree_model_get_iter(model, &iter, path))
	{
		gpointer tmp;
		gtk_tree_model_get(model, &iter, 2, &tmp, -1);
		const KnownBoard *kb = tmp;
		char group[64];
		if(boardid_to_keyfile_group(&kb->id, group, sizeof group))
		{
			GtkWidget *editor = build_editor_from_templates(cfg, group, "Board settings", board_settings);
			GtkWidget *dlg = gtk_dialog_new_with_buttons("Board settings", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
			gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dlg))), editor);
			gtk_widget_show_all(editor);
			const gint response = gtk_dialog_run(GTK_DIALOG(dlg));
			gtk_widget_destroy(dlg);
		}
	}
}

static GtkWidget * build_editor_for_boards(const Config *cfg)
{
	GtkWidget *grid = gtk_grid_new();
	GtkListStore *store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);

	GHashTableIter iter;
	g_hash_table_iter_init(&iter, cfg->known_boards);
	gpointer key, value;
	while(g_hash_table_iter_next(&iter, &key, &value))
	{
		const KnownBoard *kb = value;
		GtkTreeIter liter;
		gtk_list_store_append(store, &liter);
		gtk_list_store_set(store, &liter, 0, "<none>", 1, kb->id.board, 2, kb,-1);
	}

	GtkWidget *view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);
	column = gtk_tree_view_column_new_with_attributes("Board Model", renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);
	gtk_widget_set_hexpand(view, TRUE);
	gtk_widget_set_vexpand(view, TRUE);
	g_signal_connect(G_OBJECT(view), "row_activated", G_CALLBACK(evt_boards_row_activated), (gpointer) cfg);
	gtk_grid_attach(GTK_GRID(grid), view, 0, 0, 1, 1);

	return grid;
}

Config * config_edit(const Config *cfg, GtkWindow *parent, GuiInfo *gui)
{
	Config *editing = config_copy(cfg);

	GtkWidget *dlg = gtk_dialog_new_with_buttons("Preferences", parent, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
	GtkWidget *grid = gtk_grid_new();

	GtkWidget *global = build_editor_from_templates(editing, NULL, NULL, global_settings);
	gtk_grid_attach(GTK_GRID(grid), global, 0, 0, 1, 1);

	GtkWidget *boards = gtk_frame_new("Known boards");
	GtkWidget *be = build_editor_for_boards(cfg);
	gtk_container_add(GTK_CONTAINER(boards), be);
	gtk_grid_attach(GTK_GRID(grid), boards, 0, 1, 1, 1);

	gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dlg))), grid);

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

/* ------------------------------------------------------------------- */

static bool board_to_keyfile(Config *cfg, const SettingTemplate **templates, const KnownBoard *kb)
{
	for(const SettingTemplate *here = *templates; here != NULL; here = *++templates)
		keyfile_add_from_template(cfg->keyfile, kb->group, here);
	return true;
}

void config_update_boards(Config *cfg, const GSList *autodetected)
{
	for(; autodetected != NULL; autodetected = g_slist_next(autodetected))
	{
		const AutodetectedTarget *at = autodetected->data;
		KnownBoard *kb = g_hash_table_lookup(cfg->known_boards, &at->id);
		if(kb == NULL)
		{
			kb = g_malloc(sizeof *kb);
			kb->id = at->id;
			if(boardid_to_keyfile_group(&kb->id, kb->group, sizeof kb->group))
			{
				g_hash_table_insert(cfg->known_boards, &kb->id, kb);
				board_to_keyfile(cfg, board_settings, kb);
			}
			else
				g_error("Failed to build GKeyFile group name for BoardId");
		}
	}
}

/* ------------------------------------------------------------------- */

bool config_get_autodetect_on_startup(const Config *cfg)
{
	return cfg != NULL ? g_key_file_get_boolean(cfg->keyfile, GRP_GLOBAL, TMPL_KEY(autodetect_on_startup), NULL) : false;
}

bool config_get_autoconnect_once(const Config *cfg)
{
	return cfg != NULL ? g_key_file_get_boolean(cfg->keyfile, GRP_GLOBAL, TMPL_KEY(connect_first_on_first_autodetect), NULL) : false;
}
