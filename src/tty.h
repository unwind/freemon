/*
 * frdm-mntr TTY module.
*/

#if !defined TTY_H_
#define	TTY_H_

#include <stdbool.h>
#include <glib.h>

#include "target.h"

/* ------------------------------------------------------------------- */

typedef enum {
	TTY_TYPE_FIFO = 0,
	TTY_TYPE_SERIAL,
} TtyType;

/* Type-dependent structures embed this first. */
typedef struct {
	TtyType		type;
	const gchar	*device;
	guint		handle;
} TtyInfo;

#include "tty-fifo.h"
#include "tty-serial.h"

/* ------------------------------------------------------------------- */

TtyInfo *	tty_open(Target *target, TtyType type, const char *device);
void		tty_close(TtyInfo *tty);

#endif		/* TTY_H_ */
