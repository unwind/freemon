/*
 * frdm-mntr upload action module.
*/

#include <stdio.h>

#include "action-upload.h"

/* ------------------------------------------------------------------- */

static void evt_upload_activate(GtkAction *action, gpointer user)
{
	const char	*binary = gui_get_binary(user);
	const char	*target = gui_get_target(user);

	printf("upload, upload (%s->%s)\n", binary, target);
}

/* ------------------------------------------------------------------- */

GtkAction * action_upload_init(GuiInfo *gui)
{
	GtkAction	*upload;

	upload = gtk_action_new("upload", "Upload", "Uploads selected binary to the FRDM board.", GTK_STOCK_MEDIA_PLAY);
	g_signal_connect(G_OBJECT(upload), "activate", G_CALLBACK(evt_upload_activate), gui);

	return upload;
}
