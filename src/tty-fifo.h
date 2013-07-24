/*
 * frdm-mntr FIFO TTY module.
 *
 * Not intended to be directly included, just include "tty.h".
*/

#if !defined TTY_FIFO_H_
#define	TTY_FIFO_H_

/* ------------------------------------------------------------------- */

TtyInfo *	tty_fifo_open(const char *device, Target *target);
void		tty_fifo_close(TtyInfo *tty);

#endif		/* TTY_FIFO_H_ */
