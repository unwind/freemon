/*
 * frdm-mntr serial port TTY module.
 *
 * Not intended to be directly included, just include "tty.h".
*/

#if !defined TTY_SERIAL_H_
#define	TTY_SERIAL_H_

/* ------------------------------------------------------------------- */

TtyInfo *	tty_serial_open(GuiInfo *gui, const char *device);
void		tty_serial_close(TtyInfo *tty);

#endif		/* TTY_SERIAL_H_ */
