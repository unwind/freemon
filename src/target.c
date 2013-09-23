/*
 * Freemon: target module.
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

#include <gtk/gtk.h>
#include <vte/vte.h>

#include "action-upload.h"
#include "autodetect.h"
#include "config.h"
#include "tty.h"

#include "target.h"

/* ------------------------------------------------------------------- */

struct Target {
	BoardId		id;

	char		name[32];
	TtyInfo		*tty;
	char		upload_path[2048];

	GtkWidget	*gui;
	GtkWidget	*terminal;
	GtkWidget	*terminal_menu;

	void		(*keyhandler)(guint32 unicode, gpointer user);
	gpointer	keyhandler_user;

	GtkWidget	*binary;	// A GtkFileChooserButton.
	GtkWidget	*binary_info;	// A GtkLabel.
};

/* --------------------------------------o----------------------------- */

Target * target_new_from_autodetected(const AutodetectedTarget *at, GuiInfo *gui)
{
	Target	*target = g_malloc(sizeof *target);

	target->id = at->id;
	config_board_get_name(gui_config_get(gui), &target->id, target->name, sizeof target->name);
	g_strlcpy(target->upload_path, at->path, sizeof target->upload_path);

	target->gui = NULL;
	target->keyhandler = NULL;
	target->gui = NULL;
	target->tty = tty_open(at->device, target);

	return target;
}

void target_destroy(Target *target)
{
	tty_close(target->tty);
	g_free(target);
}

/* ------------------------------------------------------------------- */

const char * target_get_name(const Target *target)
{
	return target->name;
}

const char * target_get_device(const Target *target)
{
	return tty_get_device(target->tty);
}

const char * target_get_binary(const Target *target)
{
	if(target->gui == NULL)
		return NULL;

	gchar *fn = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(target->binary));
	if(fn != NULL)
	{
		static char	buf[4096];	/* A bit creepy perhaps, but simplifies life for the caller. FIXME: Dig up PATH_MAX. */
		const gint req = g_snprintf(buf, sizeof buf, "%s", fn);
		g_free(fn);
		return req < (sizeof buf - 1) ? buf : NULL;
	}
	return NULL;
}

const char * target_get_upload_path(const Target *target)
{
	return target->upload_path;
}

/* ------------------------------------------------------------------- */

static void evt_terminal_reset_activate(GtkWidget *wid, gpointer user)
{
	vte_terminal_reset(VTE_TERMINAL(((Target *) user)->terminal), TRUE, TRUE);
}

static gboolean evt_terminal_button_press(GtkWidget *wid, GdkEvent *event, gpointer user)
{
	const GdkEventButton	*btn = (GdkEventButton *) event;

	if(btn->button == 3)
	{
		Target	*target = user;

		gtk_menu_popup(GTK_MENU(target->terminal_menu), NULL, NULL, NULL, NULL, btn->button, btn->time);

		return TRUE;
	}
	return FALSE;
}

static gboolean evt_terminal_key_press(GtkWidget *wid, GdkEvent *event, gpointer user)
{
	Target	*target = user;

	if(target->keyhandler != NULL)
	{
		guint keyval = ((GdkEventKey *) event)->keyval;

		/* Translate troublesome keyvals. */
		if(keyval == GDK_KEY_KP_Enter)
			keyval = GDK_KEY_Return;
		/* Then look up corresponding Unicode, and hand that to the handler. */
		const guint32 unicode = gdk_keyval_to_unicode(keyval);
		if(unicode != 0)
		{
			target->keyhandler(unicode, target->keyhandler_user);
			return TRUE;
		}
	}
	return FALSE;
}

static void evt_terminal_map(GtkWidget *wid, gpointer user)
{
	gtk_widget_grab_focus(wid);
}

static void chooser_set_filter(GtkFileChooser *chooser)
{
	GtkFileFilter	*filter = gtk_file_filter_new();

	gtk_file_filter_add_pattern(filter, "*.bin");
	gtk_file_filter_add_pattern(filter, "*.srec");
	gtk_file_filter_add_pattern(filter, "*.s19");

	gtk_file_chooser_set_filter(chooser, filter);
}

static void evt_target_file_set(GtkWidget *wid, gpointer user)
{
	const Target *target = user;
	gchar buf[64] = "--";

	GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(wid));
	if(file != NULL)
	{
		GFileInfo *fi = g_file_query_info(file, G_FILE_ATTRIBUTE_STANDARD_SIZE, G_FILE_QUERY_INFO_NONE, NULL, NULL);
		if(fi != NULL)
		{
			const goffset size = g_file_info_get_size(fi);
			g_snprintf(buf, sizeof buf, "%.1f KB", size / 1024.0f);
			g_object_unref(fi);
		}
		g_object_unref(file);
	}
	gtk_label_set_text(GTK_LABEL(target->binary_info), buf);
}

GtkWidget * target_gui_create(Target *target)
{
	if(target->gui != NULL)
		return target->gui;

	target->gui = gtk_grid_new();

	target->terminal = vte_terminal_new();
	vte_terminal_set_size(VTE_TERMINAL(target->terminal), 80, 25);
	gtk_widget_add_events(target->terminal, GDK_KEY_PRESS_MASK);
	g_signal_connect(G_OBJECT(target->terminal), "button-press-event", G_CALLBACK(evt_terminal_button_press), target);
	g_signal_connect(G_OBJECT(target->terminal), "key-press-event", G_CALLBACK(evt_terminal_key_press), target);
	g_signal_connect(G_OBJECT(target->terminal), "map", G_CALLBACK(evt_terminal_map), target);
	gtk_widget_set_hexpand(target->terminal, TRUE);
	gtk_widget_set_vexpand(target->terminal, TRUE);
	gtk_widget_set_halign(target->terminal, GTK_ALIGN_FILL);
	gtk_grid_attach(GTK_GRID(target->gui), target->terminal, 0, 0, 5, 1);
	GtkWidget *scbar = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(target->terminal)));
	gtk_grid_attach(GTK_GRID(target->gui), scbar, 5, 0, 1, 1);

	target->terminal_menu = gtk_menu_new();
	GtkWidget *item = gtk_menu_item_new_with_label("Reset");
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(evt_terminal_reset_activate), target);
	gtk_menu_shell_append(GTK_MENU_SHELL(target->terminal_menu), item);
	gtk_widget_show_all(item);

	GtkWidget *label = gtk_label_new("Binary:");
	gtk_grid_attach(GTK_GRID(target->gui), label, 0, 1, 1, 1);
	target->binary = gtk_file_chooser_button_new("Select S-record or Binary for Upload", GTK_FILE_CHOOSER_ACTION_OPEN);
	chooser_set_filter(GTK_FILE_CHOOSER(target->binary));
	g_signal_connect(G_OBJECT(target->binary), "file-set", G_CALLBACK(evt_target_file_set), target);
	gtk_widget_set_hexpand(target->binary, TRUE);
	gtk_widget_set_halign(target->binary, GTK_ALIGN_FILL);
	gtk_grid_attach(GTK_GRID(target->gui), target->binary, 1, 1, 1, 1);

	target->binary_info = gtk_label_new("--");
	gtk_widget_set_margin_right(target->binary_info, 6);
	gtk_grid_attach(GTK_GRID(target->gui), target->binary_info, 2, 1, 1, 1);

	GtkWidget *btn = gtk_button_new();
	gtk_activatable_set_related_action(GTK_ACTIVATABLE(btn), action_upload_new(target));
	gtk_grid_attach(GTK_GRID(target->gui), btn, 3, 1, 1, 1);

	return target->gui;
}

void target_gui_terminal_set_keyhandler(Target *target, void (*handler)(guint32 unicode, gpointer user), gpointer user)
{
	target->keyhandler = handler;
	target->keyhandler_user = user;
}

void target_gui_terminal_insert(Target *target, const char *text, size_t length)
{
	vte_terminal_feed(VTE_TERMINAL(target->terminal), text, length);
}

void target_gui_destroy(Target *target)
{
	if(target->gui == NULL)
		return;

	gtk_widget_destroy(target->gui);
}
