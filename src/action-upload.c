/*
 * frdm-mntr upload action module.
*/

#include <stdio.h>
#include <string.h>

#include "action-upload.h"

/* ------------------------------------------------------------------- */

static gboolean do_copy(const char *src_path, const char *dst_path)
{
	GFile	*src = g_file_new_for_path(src_path), *dst = g_file_new_for_path(dst_path);

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
	const char	*blast = strrchr(binary, G_DIR_SEPARATOR);
	if(blast == NULL)
		blast = binary;
	else
		++blast;
	g_snprintf(tbuf, sizeof tbuf, "%s" G_DIR_SEPARATOR_S "%s", path, blast);

	printf("Uploading %s to %s\n", binary, tbuf);
	const gboolean ok = do_copy(binary, tbuf);
	printf(" status: %s\n", ok ? "ok" : "fail");
}

/* ------------------------------------------------------------------- */

GtkAction * action_upload_new(Target *target)
{
	GtkAction	*upload;

	upload = gtk_action_new("upload", "Upload", "Uploads selected binary to the FRDM board.", GTK_STOCK_MEDIA_PLAY);
	g_signal_connect(G_OBJECT(upload), "activate", G_CALLBACK(evt_upload_activate), target);

	return upload;
}
