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

static void chooser_set_filter(GtkFileChooser *chooser)
{
	GtkFileFilter	*filter = gtk_file_filter_new();

	gtk_file_filter_add_pattern(filter, "*.bin");
	gtk_file_filter_add_pattern(filter, "*.srec");
	gtk_file_filter_add_pattern(filter, "*.s19");

	gtk_file_chooser_set_filter(chooser, filter);
}

GtkWidget * gui_mainwindow_open(GuiInfo *gui, const Actions *actions, const char *title)
{
	GtkWidget	*win;

	if(gui == NULL)
		return NULL;

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win), title);
	gtk_container_set_border_width(GTK_CONTAINER(win), 4);
	g_signal_connect(G_OBJECT(win), "delete_event", G_CALLBACK(evt_mainwindow_delete), NULL);

	gui->grid = gtk_grid_new();
	gui->toolbar = gtk_toolbar_new();
	GtkWidget *ati = gtk_action_create_tool_item(actions->connect);
	gtk_toolbar_insert(GTK_TOOLBAR(gui->toolbar), GTK_TOOL_ITEM(ati), 0);
	gtk_grid_attach(GTK_GRID(gui->grid), gui->toolbar, 0, 0, 1, 1);

/*	label = gtk_label_new("Binary:");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
	info->binary = gtk_file_chooser_button_new("Select S-record or Binary for Upload", GTK_FILE_CHOOSER_ACTION_OPEN);
	chooser_set_filter(GTK_FILE_CHOOSER(info->binary));
	gtk_widget_set_hexpand(info->binary, TRUE);
	gtk_widget_set_halign(info->binary, GTK_ALIGN_FILL);
	gtk_grid_attach(GTK_GRID(grid), info->binary, 1, 0, 1, 1);

	label = gtk_label_new("Target:");
	gtk_grid_attach(GTK_GRID(grid), label, 2, 0, 1, 1);
	info->target = gtk_file_chooser_button_new("Select Target Directory", GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
	gtk_widget_set_hexpand(info->target, TRUE);
	gtk_widget_set_halign(info->target, GTK_ALIGN_FILL);
	gtk_grid_attach(GTK_GRID(grid), info->target, 3, 0, 1, 1);

	btn = gtk_button_new();
	gtk_activatable_set_related_action(GTK_ACTIVATABLE(btn), actions->upload);
	gtk_grid_attach(GTK_GRID(grid), btn, 4, 0, 1, 1);

	info->terminal = vte_terminal_new();
	vte_terminal_set_size(VTE_TERMINAL(info->terminal), 80, 25);
	gtk_widget_add_events(info->terminal, GDK_KEY_PRESS_MASK);
	g_signal_connect(G_OBJECT(info->terminal), "button-press-event", G_CALLBACK(evt_terminal_button_press), info);
	g_signal_connect(G_OBJECT(info->terminal), "key-press-event", G_CALLBACK(evt_terminal_key_press), info);
	g_signal_connect(G_OBJECT(info->terminal), "map", G_CALLBACK(evt_terminal_mapped), info);
	gtk_widget_set_vexpand(info->terminal, TRUE);
	gtk_grid_attach(GTK_GRID(grid), info->terminal, 0, 1, 5, 1);
	scbar = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(info->terminal)));
	gtk_grid_attach(GTK_GRID(grid), scbar, 5, 1, 1, 1);
*/
	gtk_container_add(GTK_CONTAINER(win), gui->grid);

	return win;
}

/* ------------------------------------------------------------------- */

void gui_target_add(GuiInfo *info, Target *target)
{
	GtkWidget	*ui = target_gui_create(target);

	gtk_grid_attach(GTK_GRID(info->grid), ui, 0, 1, 1, 1);
	gtk_widget_show_all(ui);
}

/* ------------------------------------------------------------------- */

static const char * filechooser_get_filename(char *buf, size_t buf_max, GtkWidget *filechooser)
{
	gchar	*fn = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser));

	if(fn != NULL)
	{
		const gint req = g_snprintf(buf, buf_max, "%s", fn);
		g_free(fn);

		return req < buf_max - 1 ? buf : NULL;
	}
	return NULL;
}

const char * gui_get_binary(const GuiInfo *gui)
{
	static char	buf[4096];	/* A bit creepy perhaps, but simplifies life for the caller. FIXME: Dig up PATH_MAX. */

	return filechooser_get_filename(buf, sizeof buf, gui->binary);
}

const char * gui_get_target(const GuiInfo *gui)
{
	static char	buf[4096];	/* FIXME: Dig up PATH_MAX. */

	return filechooser_get_filename(buf, sizeof buf, gui->target);
}
