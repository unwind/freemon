/*
 * frdm-mntr GUI module.
*/

#if !defined GUI_H_
#define	GUI_H_

#include <gtk/gtk.h>
#include <vte/vte.h>

typedef struct {
	GtkWidget	*binary;	/* A GtkFileChooserButton. */
	VteTerminal	*terminal;
} GuiInfo;

GtkWidget	* gui_mainwindow_open(GuiInfo *info, const char *title);

#endif		/* GUI_H_ */
