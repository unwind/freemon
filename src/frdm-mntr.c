/*
 *
*/

#include <stdlib.h>

#define	VERSION	"1.0"

#include "gui.h"

int main(int argc, char *argv[])
{
	GuiInfo		gui;
	GtkWidget	*mw;

	gtk_init(&argc, &argv);

	mw = gui_mainwindow_open(&gui, "frdm-mntr v" VERSION " by Emil Brink");
	gtk_widget_show_all(mw);
	gtk_main();

	return EXIT_SUCCESS;
}
