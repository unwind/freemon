/*
 * frdm-mntr GUI module.
*/

#include "gui.h"

static gboolean evt_mainwindow_delete(GtkWidget *wid, const GdkEvent *evt, gpointer user)
{
	gtk_main_quit();

	return TRUE;
}

GtkWidget * gui_mainwindow_open(const char *title)
{
	GtkWidget	*win;

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win), title);
	g_signal_connect(G_OBJECT(win), "delete_event", G_CALLBACK(evt_mainwindow_delete), NULL);

	return win;
}
