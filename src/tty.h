/*
 * frdm-mntr TTY module.
*/

#if !defined TTY_H_
#define	TTY_H_

#include <stdbool.h>
#include <glib.h>

#include "gui.h"

/* ------------------------------------------------------------------- */

typedef struct {
	const gchar	*device;
	gint		fd;
	guint		handle;
} TtyInfo;

/* ------------------------------------------------------------------- */

bool	tty_open(TtyInfo *tty, GuiInfo *gui, const char *device);
void	tty_close(TtyInfo *tty);

#endif		/* TTY_H_ */
