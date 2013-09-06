/*
 * Freemon: GUI module.
 *
 * Copyright 2013 Emil Brink <emil@obsession.se>.
 * 
 * This file is part of Freemon.
 *
 * Freemon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Freemon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Freemon.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>

#include "action-about.h"
#include "action-config.h"
#include "autodetect.h"

#include "gui.h"

/* ------------------------------------------------------------------- */

static gboolean evt_mainwindow_delete(GtkWidget *wid, const GdkEvent *evt, gpointer user)
{
	gtk_main_quit();

	return TRUE;
}

static void cb_menu_remove_child(GtkWidget *wid, gpointer user)
{
	gtk_container_remove(GTK_CONTAINER(user), wid);
}

static void autodetected_target_connect(const AutodetectedTarget *at, GuiInfo *gui)
{
	const char *rsep = strrchr(at->device, '/');
	Target *t = target_new_serial(rsep != NULL ? rsep + 1 : at->device, at->device, at->path);
	gui_target_add(gui, t);
}

/* Determine if the given detected target is already connected, i.e. has a tab open. */
static bool target_connected(const GuiInfo *gui, const AutodetectedTarget *target)
{
	GtkWidget *child;
	for(int i = 0; (child = gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui->notebook), i)) != NULL; ++i)
	{
		GtkWidget *grid = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui->notebook), child);
		/* Extract actual GtkLabel widget from the grid (for the close button). */
		GtkWidget *label = gtk_grid_get_child_at(GTK_GRID(grid), 0, 0);
		const char *ltext = gtk_label_get_text(GTK_LABEL(label));
		const char *dev = target->device;
		const char *sep = strrchr(dev, '/');
		if((sep != NULL && strcmp(sep + 1, ltext) == 0) || strcmp(dev, ltext) == 0)
			return true;
	}
	return false;
}

/* Looks up the menu item in the currently available menu of targets that matches the Target, and makes it sensitive. */
static void target_make_sensitive(GuiInfo *gui, const Target *target)
{
	GtkWidget *menu = gtk_menu_tool_button_get_menu(GTK_MENU_TOOL_BUTTON(gui->targets));
	GList *items = gtk_container_get_children(GTK_CONTAINER(menu));

	for(const GList *iter = items; iter != NULL; iter = g_list_next(iter))
	{
		GtkWidget *item = iter->data;
		const AutodetectedTarget *at = g_object_get_data(G_OBJECT(item), "target");
		if(strcmp(at->device, target_get_device(target)) == 0)
		{
			gtk_widget_set_sensitive(item, TRUE);
			break;
		}
	}
	g_list_free(items);
}

static void targets_update_sensitivity(GuiInfo *gui, const GList *items)
{
	GList *local_items = NULL;

	if(items == NULL)
	{
		GtkWidget *menu = gtk_menu_tool_button_get_menu(GTK_MENU_TOOL_BUTTON(gui->targets));
		local_items = gtk_container_get_children(GTK_CONTAINER(menu));
		items = local_items;
	}

	if(items != NULL)
	{
		bool sensitive = false;

		for(const GList *iter = items; iter != NULL; iter = g_list_next(iter))
		{
			GtkWidget *item = iter->data;
			const bool target_sensitive = gtk_widget_get_sensitive(item);
			sensitive |= target_sensitive;
		}
		gtk_widget_set_sensitive(GTK_WIDGET(gui->targets), sensitive);
	}
	if(local_items)
		g_list_free(local_items);
}

static void evt_target_activate(GtkWidget *item, gpointer user)
{
	const AutodetectedTarget *at = g_object_get_data(G_OBJECT(item), "target");
	autodetected_target_connect(at, user);
	gtk_widget_set_sensitive(item, false);
	targets_update_sensitivity(user, NULL);
}

static void evt_targets_clicked(GtkMenuToolButton *btn, gpointer user)
{
	GtkWidget *menu = gtk_menu_tool_button_get_menu(btn);
	GList *items = gtk_container_get_children(GTK_CONTAINER(menu));

	if(items != NULL)
	{
		for(GList *iter = items; iter != NULL; iter = g_list_next(iter))
		{
			GtkWidget *item = iter->data;

			if(gtk_widget_get_sensitive(item))
			{
				evt_target_activate(item, user);
				break;
			}
		}
		targets_update_sensitivity(user, items);
		g_list_free(items);
	}
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
			const bool target_sensitive = !target_connected(gui, at);
			gtk_widget_set_sensitive(tmi, target_sensitive);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), tmi);
		}
		gtk_widget_show_all(menu);
		if(gui->available_targets != NULL)
			autodetect_free(gui->available_targets);
		gui->available_targets = targets;
		targets_update_sensitivity(gui, NULL);
	}
}

GtkWidget * gui_init(GuiInfo *gui, const char *title)
{
	GtkWidget	*win;

	if(gui == NULL)
		return NULL;

	gui->config = config_init();

	gui->action_about = action_about_new();
	gui->action_config = action_config_new(gui);
	gui->available_targets = NULL;

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win), title);
	gtk_window_set_default_size(GTK_WINDOW(win), 640, 480);
	gtk_container_set_border_width(GTK_CONTAINER(win), 4);
	g_signal_connect(G_OBJECT(win), "delete_event", G_CALLBACK(evt_mainwindow_delete), NULL);

	gui->grid = gtk_grid_new();
	gui->toolbar = gtk_toolbar_new();

	GtkToolItem *btn = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH);
	gtk_widget_set_tooltip_text(GTK_WIDGET(btn), "Refreshes list of available FRDM-KL25Z boards by auto-detecting.");
	g_signal_connect(G_OBJECT(btn), "clicked", G_CALLBACK(evt_targets_refresh_clicked), gui);
	gtk_toolbar_insert(GTK_TOOLBAR(gui->toolbar), GTK_TOOL_ITEM(btn), 0);

	gui->targets = gtk_menu_tool_button_new_from_stock(GTK_STOCK_CONNECT);
	gtk_widget_set_tooltip_text(GTK_WIDGET(gui->targets), "Connects to the first available board.\nUse the pop-down menu to pick one yourself.");
	GtkWidget *tmenu = gtk_menu_new();
	gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(gui->targets), tmenu);
	g_signal_connect(G_OBJECT(gui->targets), "clicked", G_CALLBACK(evt_targets_clicked), gui);
	gtk_widget_set_sensitive(GTK_WIDGET(gui->targets), FALSE);
	gtk_toolbar_insert(GTK_TOOLBAR(gui->toolbar), GTK_TOOL_ITEM(gui->targets), 1);

	GtkToolItem *ti = gtk_separator_tool_item_new();
	gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(ti), FALSE);
	gtk_tool_item_set_expand(ti, TRUE);
	gtk_toolbar_insert(GTK_TOOLBAR(gui->toolbar), ti, 2);
	ti = GTK_TOOL_ITEM(gtk_action_create_tool_item(gui->action_config));
	gtk_toolbar_insert(GTK_TOOLBAR(gui->toolbar), ti, 3);
	ti = GTK_TOOL_ITEM(gtk_action_create_tool_item(gui->action_about));
	gtk_toolbar_insert(GTK_TOOLBAR(gui->toolbar), ti, 4);

	gtk_widget_set_hexpand(gui->toolbar, TRUE);
	gtk_widget_set_halign(gui->toolbar, GTK_ALIGN_FILL);
	gtk_grid_attach(GTK_GRID(gui->grid), gui->toolbar, 0, 0, 1, 1);

	gui->notebook = gtk_notebook_new();
	gtk_grid_attach(GTK_GRID(gui->grid), gui->notebook, 0, 1, 1, 1);

	gtk_container_add(GTK_CONTAINER(win), gui->grid);

	return win;
}

Config * gui_get_config(GuiInfo *gui)
{
	return gui->config;
}

/* ------------------------------------------------------------------- */

static void evt_target_close_clicked(GtkWidget *wid, gpointer user)
{
	GuiInfo	*gui = g_object_get_data(G_OBJECT(wid), "gui");
	Target	*target = user;
	const gint num = gtk_notebook_page_num(GTK_NOTEBOOK(gui->notebook), target_gui_create(target));
	target_make_sensitive(gui, target);
	targets_update_sensitivity(gui, NULL);
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

	GtkWidget *ui = target_gui_create(target);
	gtk_widget_show_all(ui);
	gtk_notebook_append_page(GTK_NOTEBOOK(gui->notebook), ui, grid);
}
