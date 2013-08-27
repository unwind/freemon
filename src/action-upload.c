/*
 * Freemon: upload action module.
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

#include "action-upload.h"

/* ------------------------------------------------------------------- */

static gboolean do_copy(const char *src_path, const char *dst_path)
{
	GFile *src = g_file_new_for_path(src_path), *dst = g_file_new_for_path(dst_path);

	const gboolean ok = g_file_copy(src, dst, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, NULL);

	g_object_unref(dst);
	g_object_unref(src);

	return ok;
}

static void evt_upload_activate(GtkAction *action, gpointer user)
{
	Target	*target = user;
	char		tbuf[8192];
	const char	*binary = target_get_binary(target);
	const char	*path = target_get_upload_path(target);

	if(binary == NULL || path == NULL)
		return;

	/* Construct full target filename, which needs the target appended. */
	const char *blast = strrchr(binary, G_DIR_SEPARATOR);
	if(blast == NULL)
		blast = binary;
	else
		++blast;
	g_snprintf(tbuf, sizeof tbuf, "%s" G_DIR_SEPARATOR_S "%s", path, blast);

	do_copy(binary, tbuf);	// TODO: Actually tracking and reporting any problem might be a good idea, here.
}

/* ------------------------------------------------------------------- */

GtkAction * action_upload_new(Target *target)
{
	GtkAction	*upload;

	upload = gtk_action_new("upload", "Upload", "Uploads selected binary to the FRDM board.", GTK_STOCK_MEDIA_PLAY);
	g_signal_connect(G_OBJECT(upload), "activate", G_CALLBACK(evt_upload_activate), target);

	return upload;
}
