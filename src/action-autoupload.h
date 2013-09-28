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

#if !defined ACTION_AUTO_UPLOAD_H_
#define	ACTION_AUTO_UPLOAD_H_

#include <gtk/gtk.h>

#include "gui.h"

GtkAction * action_autoupload_new(Target *target, GtkWidget *filechooser);

#endif		/* ACTION_AUTO_UPLOAD_H_ */
