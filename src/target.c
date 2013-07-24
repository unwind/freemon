/*
 * frdm-mntr target module.
*/

#include <gtk/gtk.h>
#include <vte/vte.h>

#include "tty.h"

#include "target.h"

/* ------------------------------------------------------------------- */

struct Target {
	char		name[32];
	TtyType		tty_type;
	char		tty_device[64];
	char		upload_path[2048];

	GtkWidget	*gui;
	GtkWidget	*terminal;
};

/* ------------------------------------------------------------------- */

Target * target_new_serial(const char *name, const char *tty_device, const char *upload_path)
{
	Target	*target = g_malloc(sizeof *target);

	g_strlcpy(target->name, name, sizeof target->name);
	target->tty_type = TTY_TYPE_SERIAL;
	g_strlcpy(target->tty_device, tty_device, sizeof target->tty_device);
	g_strlcpy(target->upload_path, upload_path, sizeof target->upload_path);

	target->gui = NULL;

	return target;
}

/* ------------------------------------------------------------------- */

GtkWidget * target_gui_create(Target *target)
{
	if(target->gui != NULL)
		return target->gui;

	target->gui = gtk_grid_new();
	GtkWidget *label = gtk_label_new(target->tty_device);
	gtk_grid_attach(GTK_GRID(target->gui), label, 0, 0, 1, 1);
	label = gtk_label_new(target->upload_path);
	gtk_grid_attach(GTK_GRID(target->gui), label, 1, 0, 1, 1);

	target->terminal = vte_terminal_new();
	vte_terminal_set_size(VTE_TERMINAL(target->terminal), 80, 25);
	gtk_widget_add_events(target->terminal, GDK_KEY_PRESS_MASK);
/*	g_signal_connect(G_OBJECT(target->terminal), "button-press-event", G_CALLBACK(evt_terminal_button_press), info);
	g_signal_connect(G_OBJECT(target->terminal), "key-press-event", G_CALLBACK(evt_terminal_key_press), info);
	g_signal_connect(G_OBJECT(target->terminal), "map", G_CALLBACK(evt_terminal_mapped), info);
*/	gtk_widget_set_vexpand(target->terminal, TRUE);
	gtk_grid_attach(GTK_GRID(target->gui), target->terminal, 0, 1, 5, 1);
	GtkWidget *scbar = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(target->terminal)));
	gtk_grid_attach(GTK_GRID(target->gui), scbar, 5, 1, 1, 1);

	return target->gui;
}

void target_gui_destroy(Target *target)
{
	if(target->gui == NULL)
		return;

	gtk_widget_destroy(target->gui);
}
