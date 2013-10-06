/*
 * Freemon: automatic (monitoring) upload action module.
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

#include "action-autoupload.h"

/* ------------------------------------------------------------------- */

typedef struct {
	GtkWidget	*chooser;
	char		filename[1024];
	GFile		*file;
	GFileMonitor	*monitor;
} AutouploadInfo;

/* ------------------------------------------------------------------- */

static void action_info_install(GtkAction *action, GtkWidget *fch)
{
	AutouploadInfo *ai = g_malloc(sizeof *ai);

	ai->chooser = fch;
	ai->filename[0] = '\0';
	ai->file = NULL;
	ai->monitor = NULL;

	g_object_set_data(G_OBJECT(action), "info", ai);
}

static AutouploadInfo * action_info_get(const GtkAction *action)
{
	return g_object_get_data(G_OBJECT(action), "info");
}

static bool action_monitor_install(GtkAction *action, const char *fn)
{
	GFile *f = g_file_new_for_path(fn);
	if(f != NULL)
	{
		AutouploadInfo *info = action_info_get(action);
		if((info->monitor = g_file_monitor_file(info->file, G_FILE_MONITOR_NONE, NULL, NULL)) != NULL)
		{
			info->file = f;
			return true;
		}
		g_object_unref(f);
	}
	return false;
}

static void action_monitor_uninstall(GtkAction *action)
{
	AutouploadInfo	*info = action_info_get(action);

	if(info->monitor != NULL)
	{
		g_file_monitor_cancel(info->monitor);
		g_object_unref(info->monitor);
		info->monitor = NULL;
	}
	if(info->file != NULL)
	{
		g_object_unref(info->file);
		info->file = NULL;
	}
}

/* This gets called whenever the filename in the GtkFileChooser for the binary to upload changes. */
static void action_update_filename(GtkAction *action)
{
	AutouploadInfo	*info = action_info_get(action);

	action_monitor_uninstall(action);

	gchar *fn = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(info->chooser));
	action_monitor_install(action, fn);
	g_free(fn);
}

static void evt_autoupload_toggled(GtkAction *action, gpointer user)
{
	const bool active = gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(action));
}

static void evt_chooser_file_set(GtkWidget *wid, gpointer user)
{
	GtkAction	*action = user;

	action_update_filename(action);
}

/* ------------------------------------------------------------------- */

GtkAction * action_autoupload_new(Target *target, GtkWidget *filechooserbutton)
{
	GtkToggleAction	*autoupload;

	autoupload = gtk_toggle_action_new("autoupload", "Auto-upload", "Uploads the selected binary whenever it changes.", GTK_STOCK_MEDIA_FORWARD);
	action_info_install(GTK_ACTION(autoupload), filechooserbutton);

	g_signal_connect(G_OBJECT(autoupload), "toggled", G_CALLBACK(evt_autoupload_toggled), target);
	g_signal_connect(G_OBJECT(filechooserbutton), "file_set", G_CALLBACK(evt_chooser_file_set), autoupload);

	return GTK_ACTION(autoupload);
}
