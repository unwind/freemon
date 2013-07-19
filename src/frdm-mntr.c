/*
 *
*/

#include <stdlib.h>

#define	VERSION	"1.0"

#include "gui.h"
#include "action-upload.h"

int main(int argc, char *argv[])
{
	Actions		actions;
	GuiInfo		gui;
	GtkWidget	*mw;

	gtk_init(&argc, &argv);

	actions.upload = action_upload_init();
	mw = gui_mainwindow_open(&gui, &actions, "frdm-mntr v" VERSION " by Emil Brink");
	gtk_widget_show_all(mw);
	gtk_main();

	return EXIT_SUCCESS;
}
