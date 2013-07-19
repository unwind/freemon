/*
 * frdm-mntr GUI module.
*/

#include "gui.h"

/* ------------------------------------------------------------------- */

static gboolean evt_mainwindow_delete(GtkWidget *wid, const GdkEvent *evt, gpointer user)
{
	gtk_main_quit();

	return TRUE;
}

/* ------------------------------------------------------------------- */

GtkWidget * gui_mainwindow_open(GuiInfo *info, const char *title)
{
	GtkWidget	*win, *grid, *label, *chooser;

	if(info == NULL)
		return NULL;

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win), title);
	gtk_container_set_border_width(GTK_CONTAINER(win), 10);
	g_signal_connect(G_OBJECT(win), "delete_event", G_CALLBACK(evt_mainwindow_delete), NULL);

	grid = gtk_grid_new();
	label = gtk_label_new("Binary");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
	info->filename = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid), info->filename, 1, 0, 1, 1);
	chooser = gtk_file_chooser_button_new("Select Binary", GTK_FILE_CHOOSER_ACTION_OPEN);
	gtk_grid_attach(GTK_GRID(grid), chooser, 2, 0, 1, 1);

	gtk_container_add(GTK_CONTAINER(win), grid);

	return win;
}
