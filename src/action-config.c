/*
 * Freemon: config window action module.
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

#include "action-config.h"
#include "config.h"
#include "gui.h"

/* ------------------------------------------------------------------- */

static void evt_config_activate(GtkAction *action, gpointer user)
{
	GuiInfo *gui = user;
	const Config *old = gui_config_get(gui);
	Config *new_cfg = config_edit(old, NULL, gui);
	if(new_cfg != NULL)
		gui_config_set(gui, new_cfg);
}

/* ------------------------------------------------------------------- */

GtkAction * action_config_new(GuiInfo *gui)
{
	GtkAction *config = gtk_action_new("preferences", "Preferences", "Edits program preferences.", GTK_STOCK_PREFERENCES);

	g_signal_connect(G_OBJECT(config), "activate", G_CALLBACK(evt_config_activate), gui);

	return config;
}
