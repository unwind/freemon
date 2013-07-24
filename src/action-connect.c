/*
 * frdm-mntr connect action module.
*/

#include <stdio.h>
#include <string.h>

#include "action-connect.h"

/* ------------------------------------------------------------------- */

static void evt_connect_activate(GtkAction *action, gpointer user)
{
	Target *t = target_new_serial("My FRDM", "/dev/ttyACM0", "/media/emil/FRDM-KL25Z");
	gui_target_add(user, t);
}

/* ------------------------------------------------------------------- */

GtkAction * action_connect_init(GuiInfo *gui)
{
	GtkAction	*upload;

	upload = gtk_action_new("connect", "Connect", "Opens a connection to a FRDM board.", GTK_STOCK_CONNECT);
	g_signal_connect(G_OBJECT(upload), "activate", G_CALLBACK(evt_connect_activate), gui);

	return upload;
}
