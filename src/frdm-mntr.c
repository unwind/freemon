/*
 *
*/

#include <stdlib.h>

#define	VERSION	"1.0"

#include "action-upload.h"
#include "gui.h"
#include "tty.h"

int main(int argc, char *argv[])
{
	Actions		actions;
	GuiInfo		gui;
	TtyInfo		tty;
	GtkWidget	*mw;

	gtk_init(&argc, &argv);

	actions.upload = action_upload_init();
	mw = gui_mainwindow_open(&gui, &actions, "frdm-mntr v" VERSION " by Emil Brink");
	tty_open(&tty, &gui, "/home/emil/tmp/mntr-pipe");
	gtk_widget_show_all(mw);
	gtk_main();

	return EXIT_SUCCESS;
}
