/*
 * Freemon: GUI module.
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

#if !defined GUI_H_
#define	GUI_H_

#include <gtk/gtk.h>
#include <vte/vte.h>

#include "config.h"
#include "target.h"

typedef struct {
	Config		*config;

	GtkAction	*action_about;
	GtkAction	*action_config;

	GSList		*available_targets;	/* From autodetect_all(). */
	GtkWidget	*toolbar;
	GtkToolItem	*targets;
	GtkWidget	*notebook;

	GtkWidget	*binary;	/* A GtkFileChooserButton. */
	GtkWidget	*target;	/* Another GtkFileChooserButton. */
	GtkWidget	*terminal;	/* A VteTerminal. */
	GtkWidget	*grid;
	void		(*keyhandler)(guint32 unicode, gpointer user);
	gpointer	keyhandler_user;
	GtkWidget	*terminal_menu;	/* A GtkMenu. */
} GuiInfo;

GtkWidget *	gui_init(GuiInfo *gui, const char *title);

Config *	gui_get_config(GuiInfo *gui);

void		gui_target_add(GuiInfo *gui, Target *target);

void		gui_terminal_insert(GuiInfo *gui, const char *text, size_t length);

#endif		/* GUI_H_ */
