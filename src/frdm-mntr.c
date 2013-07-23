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
	TtyInfo		*tty;
	GtkWidget	*mw;

	gtk_init(&argc, &argv);

	actions.upload = action_upload_init(&gui);
	mw = gui_mainwindow_open(&gui, &actions, "frdm-mntr v" VERSION " by Emil Brink");
	tty = tty_open(&gui, TTY_TYPE_SERIAL, "/dev/ttyACM0");

	gtk_widget_show_all(mw);
	gtk_main();
	gtk_widget_destroy(mw);

	tty_close(tty);

	return EXIT_SUCCESS;
}
