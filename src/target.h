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

#if !defined TARGET_H_
#define	TARGET_H_

#include <gtk/gtk.h>

#include "actions.h"

typedef struct Target	Target;

Target *	target_new_serial(const char *name, const char *tty_device, const char *upload_path);
void		target_destroy(Target *target);

const char *	target_get_name(const Target *target);
const char *	target_get_device(const Target *target);
const char *	target_get_binary(const Target *target);
const char *	target_get_upload_path(const Target *target);

void		target_gui_terminal_set_keyhandler(Target *target, void (*handler)(guint32 unicode, gpointer user), gpointer user);
void		target_gui_terminal_insert(Target *target, const char *text, size_t length);

GtkWidget *	target_gui_create(Target *target, const Actions *actions);
void		target_gui_destroy(Target *target);

#endif		/* TARGET_H_ */
