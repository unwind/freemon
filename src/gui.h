/*
 * frdm-mntr GUI module.
*/

#if !defined GUI_H_
#define	GUI_H_

#include <gtk/gtk.h>
#include <vte/vte.h>

#include "actions.h"

typedef struct {
	GtkWidget	*binary;	/* A GtkFileChooserButton. */
	GtkWidget	*target;	/* Another GtkFileChooserButton. */
	GtkWidget	*terminal;	/* A VteTerminal. */
	void		(*keyhandler)(guint32 unicode, gpointer user);
	gpointer	keyhandler_user;
	GtkWidget	*terminal_menu;	/* A GtkMenu. */
} GuiInfo;

GtkWidget *	gui_mainwindow_open(GuiInfo *info, const Actions *actions, const char *title);

const char *	gui_get_binary(const GuiInfo *info);
const char *	gui_get_target(const GuiInfo *info);

void		gui_terminal_set_keyhandler(GuiInfo *info, void (*handler)(guint32 unicode, gpointer user), gpointer user);
void		gui_terminal_insert(GuiInfo *info, const char *text, size_t length);

#endif		/* GUI_H_ */
