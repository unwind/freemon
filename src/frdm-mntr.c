/*
 *
*/

#include <stdlib.h>

#define	VERSION	"1.0"

#include "action-connect.h"
#include "action-upload.h"
#include "autodetect.h"
#include "gui.h"
#include "target.h"
#include "tty.h"

int main(int argc, char *argv[])
{
	Actions		actions;
	GuiInfo		gui;
	GtkWidget	*mw;

	gtk_init(&argc, &argv);

	actions.connect = action_connect_init(&gui);
	mw = gui_mainwindow_open(&gui, &actions, "frdm-mntr v" VERSION " by Emil Brink");

	gtk_widget_show_all(mw);
	gtk_main();
	gtk_widget_destroy(mw);

	return EXIT_SUCCESS;
}
