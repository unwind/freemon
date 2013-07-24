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
	TtyInfo		*tty;
	char		upload_path[2048];

	GtkWidget	*gui;
	GtkWidget	*terminal;
	GtkWidget	*terminal_menu;

	void		(*keyhandler)(guint32 unicode, gpointer user);
	gpointer	keyhandler_user;
};

/* ------------------------------------------------------------------- */

Target * target_new_serial(const char *name, const char *tty_device, const char *upload_path)
{
	Target	*target = g_malloc(sizeof *target);

	g_strlcpy(target->name, name, sizeof target->name);
	g_strlcpy(target->upload_path, upload_path, sizeof target->upload_path);

	target->gui = NULL;
	target->keyhandler = NULL;
	target->gui = target_gui_create(target);
	target->tty = tty_open(target, TTY_TYPE_SERIAL, tty_device);

	return target;
}

/* ------------------------------------------------------------------- */

static void evt_terminal_reset_activate(GtkWidget *wid, gpointer user)
{
	vte_terminal_reset(VTE_TERMINAL(((Target *) user)->terminal), TRUE, TRUE);
}

static gboolean evt_terminal_button_press(GtkWidget *wid, GdkEvent *event, gpointer user)
{
	const GdkEventButton	*btn = (GdkEventButton *) event;

	if(btn->button == 3)
	{
		Target	*target = user;

		gtk_menu_popup(GTK_MENU(target->terminal_menu), NULL, NULL, NULL, NULL, btn->button, btn->time);

		return TRUE;
	}
	return FALSE;
}

static gboolean evt_terminal_key_press(GtkWidget *wid, GdkEvent *event, gpointer user)
{
	Target	*target = user;

	if(target->keyhandler != NULL)
	{
		const guint32	unicode = gdk_keyval_to_unicode(((GdkEventKey *) event)->keyval);

		if(unicode != 0)
		{
			target->keyhandler(unicode, target->keyhandler_user);
			return TRUE;
		}
	}
	return FALSE;
}

GtkWidget * target_gui_create(Target *target)
{
	if(target->gui != NULL)
		return target->gui;

	target->gui = gtk_grid_new();
	GtkWidget *label = gtk_label_new("(device)");
	gtk_grid_attach(GTK_GRID(target->gui), label, 0, 0, 1, 1);
	label = gtk_label_new(target->upload_path);
	gtk_grid_attach(GTK_GRID(target->gui), label, 1, 0, 1, 1);

	target->terminal = vte_terminal_new();
	vte_terminal_set_size(VTE_TERMINAL(target->terminal), 80, 25);
	gtk_widget_add_events(target->terminal, GDK_KEY_PRESS_MASK);
	g_signal_connect(G_OBJECT(target->terminal), "button-press-event", G_CALLBACK(evt_terminal_button_press), target);
	g_signal_connect(G_OBJECT(target->terminal), "key-press-event", G_CALLBACK(evt_terminal_key_press), target);
/*	g_signal_connect(G_OBJECT(target->terminal), "map", G_CALLBACK(evt_terminal_mapped), info);
*/	gtk_widget_set_vexpand(target->terminal, TRUE);
	gtk_grid_attach(GTK_GRID(target->gui), target->terminal, 0, 1, 5, 1);
	GtkWidget *scbar = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(target->terminal)));
	gtk_grid_attach(GTK_GRID(target->gui), scbar, 5, 1, 1, 1);

	target->terminal_menu = gtk_menu_new();
	GtkWidget *item = gtk_menu_item_new_with_label("Reset");
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(evt_terminal_reset_activate), target);
	gtk_menu_shell_append(GTK_MENU_SHELL(target->terminal_menu), item);
	gtk_widget_show_all(item);

	return target->gui;
}

void target_gui_terminal_set_keyhandler(Target *target, void (*handler)(guint32 unicode, gpointer user), gpointer user)
{
	target->keyhandler = handler;
	target->keyhandler_user = user;
}

void target_gui_terminal_insert(Target *target, const char *text, size_t length)
{
	vte_terminal_feed(VTE_TERMINAL(target->terminal), text, length);
}

void target_gui_destroy(Target *target)
{
	if(target->gui == NULL)
		return;

	gtk_widget_destroy(target->gui);
}
