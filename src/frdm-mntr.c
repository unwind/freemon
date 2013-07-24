/*
 *
*/

#include <stdlib.h>

#define	VERSION	"1.0"

#include "action-upload.h"
#include "gui.h"
#include "target.h"
#include "tty.h"

int main(int argc, char *argv[])
{
	Actions		actions;
	GuiInfo		gui;
	GtkWidget	*mw;

	gtk_init(&argc, &argv);

	actions.upload = action_upload_init(&gui);
	mw = gui_mainwindow_open(&gui, &actions, "frdm-mntr v" VERSION " by Emil Brink");
	Target *t = target_new_serial("My FRDM", "/dev/ttyACM0", "/media/emil/FRDM-KL25Z");
	gui_target_add(&gui, t);
//	tty = tty_open(&gui, TTY_TYPE_SERIAL, "/dev/ttyACM0");

	gtk_widget_show_all(mw);
	gtk_main();
	gtk_widget_destroy(mw);

//	tty_close(tty);

	return EXIT_SUCCESS;
}
