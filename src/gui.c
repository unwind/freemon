/*
 * frdm-mntr GUI module.
*/

#include "gui.h"

GtkWidget * gui_mainwindow_open(const char *title)
{
	GtkWidget	*win;

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win), title);

	return win;
}
