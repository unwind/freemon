/*
 * frdm-mntr GUI module.
*/

#include <string.h>

#include "autodetect.h"

#include "gui.h"

/* ------------------------------------------------------------------- */

const Actions * gui_get_actions(const GuiInfo *gui)
{
	return gui->actions;
}

static gboolean evt_mainwindow_delete(GtkWidget *wid, const GdkEvent *evt, gpointer user)
{
	gtk_main_quit();

	return TRUE;
}

static void cb_menu_remove_child(GtkWidget *wid, gpointer user)
{
	gtk_container_remove(GTK_CONTAINER(user), wid);
}

static void evt_target_activate(GtkWidget *item, gpointer user)
{
	const AutodetectedTarget *at = g_object_get_data(G_OBJECT(item), "target");

	const char *rsep = strrchr(at->device, '/');
	Target *t = target_new_serial(rsep != NULL ? rsep + 1 : at->device, at->device, at->path);
	gui_target_add(user, t);
}

static void evt_targets_refresh_clicked(GtkToolButton *btn, gpointer user)
{
	GuiInfo *gui = user;
	GtkWidget *menu = gtk_menu_tool_button_get_menu(GTK_MENU_TOOL_BUTTON(gui->targets));

	gtk_container_foreach(GTK_CONTAINER(menu), cb_menu_remove_child, menu);
	GSList *targets = autodetect_all();
	if(targets != NULL)
	{
		for(GSList *iter = targets; iter != NULL; iter = g_slist_next(iter))
		{
			const AutodetectedTarget *at = iter->data;
			gchar buf[128];
			g_snprintf(buf, sizeof buf, "%s (on %s)", at->device, at->path);
			GtkWidget *tmi = gtk_menu_item_new_with_label(buf);
			g_object_set_data(G_OBJECT(tmi), "target", (gpointer) at);
			g_signal_connect(G_OBJECT(tmi), "activate", G_CALLBACK(evt_target_activate), gui);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), tmi);
		}
		gtk_widget_show_all(menu);
		if(gui->available_targets != NULL)
			autodetect_free(gui->available_targets);
		gui->available_targets = targets;
	}
}

GtkWidget * gui_mainwindow_open(GuiInfo *gui, const Actions *actions, const char *title)
{
	GtkWidget	*win;

	if(gui == NULL)
		return NULL;

	gui->actions = actions;
	gui->available_targets = NULL;

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win), title);
	gtk_window_set_default_size(GTK_WINDOW(win), 640, 480);
	gtk_container_set_border_width(GTK_CONTAINER(win), 4);
	g_signal_connect(G_OBJECT(win), "delete_event", G_CALLBACK(evt_mainwindow_delete), NULL);

	gui->grid = gtk_grid_new();
	gui->toolbar = gtk_toolbar_new();

	gui->targets = gtk_menu_tool_button_new_from_stock(GTK_STOCK_CONNECT);//(NULL, "Connect");
	GtkWidget *tmenu = gtk_menu_new();
	gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(gui->targets), tmenu);
	gtk_toolbar_insert(GTK_TOOLBAR(gui->toolbar), GTK_TOOL_ITEM(gui->targets), 0);
	GtkToolItem *btn = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH);
	g_signal_connect(G_OBJECT(btn), "clicked", G_CALLBACK(evt_targets_refresh_clicked), gui);
	gtk_toolbar_insert(GTK_TOOLBAR(gui->toolbar), GTK_TOOL_ITEM(btn), 1);

	gtk_widget_set_hexpand(gui->toolbar, TRUE);
	gtk_widget_set_halign(gui->toolbar, GTK_ALIGN_FILL);
	gtk_grid_attach(GTK_GRID(gui->grid), gui->toolbar, 0, 0, 1, 1);

	gui->notebook = gtk_notebook_new();
	gtk_grid_attach(GTK_GRID(gui->grid), gui->notebook, 0, 1, 1, 1);

	gtk_container_add(GTK_CONTAINER(win), gui->grid);

	return win;
}

/* ------------------------------------------------------------------- */

static void evt_target_close_clicked(GtkWidget *wid, gpointer user)
{
	GuiInfo	*gui = g_object_get_data(G_OBJECT(wid), "gui");
	Target	*target = user;
	const gint num = gtk_notebook_page_num(GTK_NOTEBOOK(gui->notebook), target_gui_create(target, NULL));
	target_destroy(target);
	gtk_notebook_remove_page(GTK_NOTEBOOK(gui->notebook), num);
}

void gui_target_add(GuiInfo *gui, Target *target)
{
	GtkWidget *grid = gtk_grid_new();
	GtkWidget *lab = gtk_label_new(target_get_name(target));
	gtk_grid_attach(GTK_GRID(grid), lab, 0, 0, 1, 1);
	/* Create a close button, using stock image but smaller. Semi-tricky. */
	GtkWidget *btn = gtk_button_new();
	GtkWidget *icon = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_button_set_image(GTK_BUTTON(btn), icon);
	gtk_button_set_always_show_image(GTK_BUTTON(btn), TRUE);
	g_object_set_data(G_OBJECT(btn), "gui", gui);
	g_signal_connect(G_OBJECT(btn), "clicked", G_CALLBACK(evt_target_close_clicked), target);
	gtk_grid_attach(GTK_GRID(grid), btn, 1, 0, 1, 1);
	gtk_widget_show_all(grid);

	GtkWidget *ui = target_gui_create(target, gui_get_actions(gui));
	gtk_widget_show_all(ui);
	gtk_notebook_append_page(GTK_NOTEBOOK(gui->notebook), ui, grid);
}
