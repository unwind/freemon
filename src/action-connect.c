/*
 * frdm-mntr connect action module.
*/

#include <stdio.h>
#include <string.h>

#include "action-connect.h"

/* ------------------------------------------------------------------- */

/*
	Target *t = target_new_serial("My FRDM", "/dev/ttyACM0", "/media/emil/FRDM-KL25Z");
	gui_target_add(user, t);
*/

static void evt_connect_activate(GtkAction *action, gpointer user)
{
	GtkWidget *dlg = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(dlg), "Connect to FRDM board");

	GtkWidget *grid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
	GtkWidget *label = gtk_label_new("Name:");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
	GtkWidget *name = gtk_entry_new();
	gtk_widget_set_hexpand(name, TRUE);
	gtk_widget_set_halign(name, GTK_ALIGN_FILL);
	gtk_grid_attach(GTK_GRID(grid), name, 1, 0, 1, 1);

	label = gtk_label_new("Serial Device:");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
	GtkWidget *device = gtk_entry_new();
	gtk_entry_set_icon_from_stock(GTK_ENTRY(device), GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_FIND);
	gtk_grid_attach(GTK_GRID(grid), device, 1, 1, 1, 1);

	label = gtk_label_new("Mounted As:");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
	GtkWidget *upload = gtk_file_chooser_button_new("Select Upload Directory", GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
	gtk_widget_set_hexpand(upload, TRUE);
	gtk_widget_set_halign(upload, GTK_ALIGN_FILL);
	gtk_grid_attach(GTK_GRID(grid), upload, 1, 2, 1, 1);

	gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dlg))), grid);
	gtk_widget_show_all(grid);

	gtk_dialog_add_button(GTK_DIALOG(dlg), GTK_STOCK_OK, GTK_RESPONSE_OK);
	gtk_dialog_add_button(GTK_DIALOG(dlg), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);

	const gint response = gtk_dialog_run(GTK_DIALOG(dlg));

	gtk_widget_destroy(dlg);
}

/* ------------------------------------------------------------------- */

GtkAction * action_connect_init(GuiInfo *gui)
{
	GtkAction	*upload;

	upload = gtk_action_new("connect", "Connect", "Opens a connection to a FRDM board.", GTK_STOCK_CONNECT);
	g_signal_connect(G_OBJECT(upload), "activate", G_CALLBACK(evt_connect_activate), gui);

	return upload;
}
