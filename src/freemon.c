/*
 * Freemon: main entrypoint module.
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

#include <stdlib.h>

#include "autodetect.h"
#include "gui.h"
#include "target.h"
#include "tty.h"

int main(int argc, char *argv[])
{
	GuiInfo		*gui;
	GtkWidget	*mw;

	gtk_init(&argc, &argv);

	gui = gui_init("Freemon v" VERSION " by Emil Brink");

	mw = gui_mainwindow_get(gui);
	gtk_widget_show_all(mw);

	gtk_main();

	gtk_widget_destroy(mw);
	config_save(gui_config_get(gui));

	return EXIT_SUCCESS;
}
