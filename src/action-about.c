/*
 * Freemon: about window action module.
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

#include "action-about.h"

/* ------------------------------------------------------------------- */

static void evt_about_activate(GtkAction *action, gpointer user)
{
}

/* ------------------------------------------------------------------- */

GtkAction * action_about_new(void)
{
	GtkAction *about = gtk_action_new("about", "About", "Displays information about this program.", GTK_STOCK_ABOUT);

	g_signal_connect(G_OBJECT(about), "activate", G_CALLBACK(evt_about_activate), NULL);

	return about;
}
