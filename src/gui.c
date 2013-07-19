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

GtkWidget * gui_mainwindow_open(GuiInfo *info, const Actions *actions, const char *title)
{
	GtkWidget	*win, *grid, *btn, *label;

	if(info == NULL)
		return NULL;

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win), title);
	gtk_container_set_border_width(GTK_CONTAINER(win), 10);
	g_signal_connect(G_OBJECT(win), "delete_event", G_CALLBACK(evt_mainwindow_delete), NULL);

	grid = gtk_grid_new();
	label = gtk_label_new("Binary");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
	info->binary = gtk_file_chooser_button_new("Select Binary", GTK_FILE_CHOOSER_ACTION_OPEN);
	gtk_widget_set_hexpand(info->binary, TRUE);
	gtk_widget_set_halign(info->binary, GTK_ALIGN_FILL);
	gtk_grid_attach(GTK_GRID(grid), info->binary, 1, 0, 1, 1);
	btn = gtk_button_new();
	gtk_activatable_set_related_action(GTK_ACTIVATABLE(btn), actions->upload);
	gtk_grid_attach(GTK_GRID(grid), btn, 2, 0, 1, 1);

	info->terminal = vte_terminal_new();
	gtk_grid_attach(GTK_GRID(grid), info->terminal, 0, 1, 3, 1);

	gtk_container_add(GTK_CONTAINER(win), grid);

	return win;
}
