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
	GtkWidget	*terminal;	/* A VteTerminal. */
} GuiInfo;

GtkWidget *	gui_mainwindow_open(GuiInfo *info, const Actions *actions, const char *title);

#endif		/* GUI_H_ */
