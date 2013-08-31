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

static void evt_about_response(GtkDialog *dlg, gint response, gpointer user)
{
	gtk_widget_destroy(GTK_WIDGET(dlg));
}

static void evt_about_activate(GtkAction *action, gpointer user)
{
	GtkAboutDialog *about = GTK_ABOUT_DIALOG(gtk_about_dialog_new());

	gtk_about_dialog_set_program_name(about, "Freemon");
	gtk_about_dialog_set_version(about, VERSION);
	gtk_about_dialog_set_copyright(about, "Copyright 2013 Emil Brink");
	gtk_about_dialog_set_license_type(about, GTK_LICENSE_GPL_3_0);
	gtk_about_dialog_set_website(about, "https://github.com/unwind/freemon");
	gtk_about_dialog_set_website_label(about, "Follow Freemon on GitHub!");
	const char *authors[] = { "Emil Brink <emil@obsession.se>", NULL };
	gtk_about_dialog_set_authors(about, authors);

	gtk_dialog_set_default_response(GTK_DIALOG(about), GTK_RESPONSE_CLOSE);
	g_signal_connect(G_OBJECT(about), "response", G_CALLBACK(evt_about_response), NULL);

	gtk_widget_show_all(about);
}

/* ------------------------------------------------------------------- */

GtkAction * action_about_new(void)
{
	GtkAction *about = gtk_action_new("about", "About", "Displays information about this program.", GTK_STOCK_ABOUT);

	g_signal_connect(G_OBJECT(about), "activate", G_CALLBACK(evt_about_activate), NULL);

	return about;
}
