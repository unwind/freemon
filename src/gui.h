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

#include "freemon.h"
#include "target.h"

GuiInfo *	gui_init(const char *title);

GtkWidget *	gui_mainwindow_get(GuiInfo *gui);

Config *	gui_config_get(GuiInfo *gui);
void		gui_config_set(GuiInfo *gui, Config *cfg);

void		gui_target_add(GuiInfo *gui, Target *target);

void		gui_terminal_insert(GuiInfo *gui, const char *text, size_t length);

#endif		/* GUI_H_ */
