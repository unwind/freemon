/*
 * frdm-mntr connect action module.
*/

#include <stdio.h>
#include <string.h>

#include "action-connect.h"

/* ------------------------------------------------------------------- */

#if 0
static void evt_upload_activate(GtkAction *action, gpointer user)
{
	char		tbuf[8192];
	const char	*binary = gui_get_binary(user);
	const char	*target = gui_get_target(user);

	if(binary == NULL || target == NULL)
		return;

	/* Construct full target filename, which needs the target appended. */
	const char	*blast = strrchr(binary, G_DIR_SEPARATOR);
	if(blast == NULL)
		blast = binary;
	else
		++blast;
	g_snprintf(tbuf, sizeof tbuf, "%s" G_DIR_SEPARATOR_S "%s", target, blast);

	printf("Uploading %s to %s\n", binary, tbuf);
	const gboolean ok = do_copy(binary, tbuf);
	printf(" status: %s\n", ok ? "ok" : "fail");
}
#endif

/* ------------------------------------------------------------------- */

GtkAction * action_connect_init(GuiInfo *gui)
{
	GtkAction	*upload;

	upload = gtk_action_new("connect", "Connect", "Opens a connection to a FRDM board.", GTK_STOCK_CONNECT);
	//g_signal_connect(G_OBJECT(upload), "activate", G_CALLBACK(evt_upload_activate), gui);

	return upload;
}
