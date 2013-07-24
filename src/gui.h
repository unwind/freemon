/*
 * frdm-mntr GUI module.
*/

#if !defined GUI_H_
#define	GUI_H_

#include <gtk/gtk.h>
#include <vte/vte.h>

#include "actions.h"
#include "target.h"

typedef struct {
	const Actions	*actions;
	GtkWidget	*toolbar;
	GtkWidget	*notebook;

	GtkWidget	*binary;	/* A GtkFileChooserButton. */
	GtkWidget	*target;	/* Another GtkFileChooserButton. */
	GtkWidget	*terminal;	/* A VteTerminal. */
	GtkWidget	*grid;
	void		(*keyhandler)(guint32 unicode, gpointer user);
	gpointer	keyhandler_user;
	GtkWidget	*terminal_menu;	/* A GtkMenu. */
} GuiInfo;

const Actions *	gui_get_actions(const GuiInfo *gui);

GtkWidget *	gui_mainwindow_open(GuiInfo *info, const Actions *actions, const char *title);

void		gui_target_add(GuiInfo *gui, Target *target);

const char *	gui_get_binary(const GuiInfo *info);
const char *	gui_get_target(const GuiInfo *info);

void		gui_terminal_insert(GuiInfo *info, const char *text, size_t length);

#endif		/* GUI_H_ */
