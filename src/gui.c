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

static gboolean evt_terminal_key_pressed(GtkWidget *wid, GdkEvent *event, gpointer user)
{
	GuiInfo	*gui = user;

	if(gui->keyhandler != NULL)
	{
		const guint32	unicode = gdk_keyval_to_unicode(((GdkEventKey *) event)->keyval);

		if(unicode != 0)
		{
			gui->keyhandler(unicode, gui->keyhandler_user);
		}
	}
	return TRUE;
}

static void chooser_set_filter(GtkFileChooser *chooser)
{
	GtkFileFilter	*filter = gtk_file_filter_new();

	gtk_file_filter_add_pattern(filter, "*.srec");
	gtk_file_filter_add_pattern(filter, "*.s19");

	gtk_file_chooser_set_filter(chooser, filter);
}

GtkWidget * gui_mainwindow_open(GuiInfo *info, const Actions *actions, const char *title)
{
	GtkWidget	*win, *grid, *btn, *label, *scwin;

	if(info == NULL)
		return NULL;

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win), title);
	gtk_container_set_border_width(GTK_CONTAINER(win), 10);
	g_signal_connect(G_OBJECT(win), "delete_event", G_CALLBACK(evt_mainwindow_delete), NULL);

	grid = gtk_grid_new();
	label = gtk_label_new("Binary");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
	info->binary = gtk_file_chooser_button_new("Select S-record For Upload", GTK_FILE_CHOOSER_ACTION_OPEN);
	chooser_set_filter(GTK_FILE_CHOOSER(info->binary));
	gtk_widget_set_hexpand(info->binary, TRUE);
	gtk_widget_set_halign(info->binary, GTK_ALIGN_FILL);
	gtk_grid_attach(GTK_GRID(grid), info->binary, 1, 0, 1, 1);
	btn = gtk_button_new();
	gtk_activatable_set_related_action(GTK_ACTIVATABLE(btn), actions->upload);
	gtk_grid_attach(GTK_GRID(grid), btn, 2, 0, 1, 1);

	info->terminal = vte_terminal_new();
	gtk_widget_add_events(info->terminal, GDK_KEY_PRESS_MASK);
	g_signal_connect(G_OBJECT(info->terminal), "key-press-event", G_CALLBACK(evt_terminal_key_pressed), info);
	vte_terminal_set_size(VTE_TERMINAL(info->terminal), 80, 25);
	gtk_widget_set_vexpand(info->terminal, TRUE);
	scwin = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scwin), info->terminal);
	const glong cw = vte_terminal_get_char_width(VTE_TERMINAL(info->terminal));
	const glong ch = vte_terminal_get_char_width(VTE_TERMINAL(info->terminal));
	gtk_widget_set_size_request(scwin, 80 * cw, 25 * ch);
	gtk_grid_attach(GTK_GRID(grid), scwin, 0, 1, 3, 1);

	gtk_container_add(GTK_CONTAINER(win), grid);

	return win;
}

void gui_terminal_set_keyhandler(GuiInfo *gui, void (*handler)(guint32 unicode, gpointer user), gpointer user)
{
	gui->keyhandler = handler;
	gui->keyhandler_user = user;
}

void gui_terminal_insert(GuiInfo *gui, const char *text, size_t length)
{
	vte_terminal_feed(VTE_TERMINAL(gui->terminal), text, length);
}
