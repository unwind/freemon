/*
 * frdm-mntr connect action module.
*/

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "action-connect.h"
#include "autodetect.h"

/* ------------------------------------------------------------------- */

typedef struct {
	GtkWidget	*name;
	GtkWidget	*device;
	GtkWidget	*path;
	GtkWidget	*dialog;
} ConnectInfo;

/* ------------------------------------------------------------------- */

/*
	Target *t = target_new_serial("My FRDM", "/dev/ttyACM0", "/media/emil/FRDM-KL25Z");
	gui_target_add(user, t);
*/

static void evaluate_entries(const ConnectInfo *info)
{
	const bool	device_set = gtk_entry_get_text_length(GTK_ENTRY(info->device)) > 0;
	const bool	path_set = gtk_entry_get_text_length(GTK_ENTRY(info->device)) > 0;

	gtk_dialog_set_response_sensitive(GTK_DIALOG(info->dialog), GTK_RESPONSE_OK, device_set && path_set);
}

static void evt_device_changed(GtkWidget *wid, gpointer user)
{
	evaluate_entries(user);
}

static void evt_path_changed(GtkWidget *wid, gpointer user)
{
	evaluate_entries(user);
}

static void evt_autodetect_clicked(GtkWidget *wid, gpointer user)
{
	autodetect_simple(NULL, NULL);
}

static void evt_connect_activate(GtkAction *action, gpointer user)
{
	ConnectInfo	info;

	info.dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(info.dialog), "Connect to FRDM board");

	GtkWidget *grid = gtk_grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(grid), 6);
	GtkWidget *label = gtk_label_new("Name:");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
	info.name = gtk_entry_new();
	gtk_widget_set_hexpand(info.name, TRUE);
	gtk_widget_set_halign(info.name, GTK_ALIGN_FILL);
	gtk_grid_attach(GTK_GRID(grid), info.name, 1, 0, 2, 1);

	label = gtk_label_new("Serial Device:");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
	info.device = gtk_entry_new();
	g_signal_connect(G_OBJECT(info.device), "changed", G_CALLBACK(evt_device_changed), &info);
	gtk_grid_attach(GTK_GRID(grid), info.device, 1, 1, 1, 1);

	label = gtk_label_new("Mounted As:");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
	info.path = gtk_file_chooser_button_new("Select Upload Directory", GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
	gtk_widget_set_hexpand(info.path, TRUE);
	gtk_widget_set_halign(info.path, GTK_ALIGN_FILL);
	g_signal_connect(G_OBJECT(info.path), "file-set", G_CALLBACK(evt_path_changed), &info);
	gtk_grid_attach(GTK_GRID(grid), info.path, 1, 2, 1, 1);

	GtkWidget *detect = gtk_button_new_with_label("Auto-\nDetect");
	g_signal_connect(G_OBJECT(detect), "clicked", G_CALLBACK(evt_autodetect_clicked), &info);
	gtk_grid_attach(GTK_GRID(grid), detect, 2, 1, 1, 2);

	gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(info.dialog))), grid);

	gtk_dialog_add_button(GTK_DIALOG(info.dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);
	gtk_dialog_add_button(GTK_DIALOG(info.dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);

	gtk_widget_show_all(grid);
	evaluate_entries(&info);

	const gint response = gtk_dialog_run(GTK_DIALOG(info.dialog));

	gtk_widget_destroy(info.dialog);
}

/* ------------------------------------------------------------------- */

GtkAction * action_connect_init(GuiInfo *gui)
{
	GtkAction	*upload;

	upload = gtk_action_new("connect", "Connect", "Opens a connection to a FRDM board.", GTK_STOCK_CONNECT);
	g_signal_connect(G_OBJECT(upload), "activate", G_CALLBACK(evt_connect_activate), gui);

	return upload;
}
