/*
 * frdm-mntr auto-detect module.
*/

#if !defined AUTODETECT_H_
#define	AUTODETECT_H_

#include <stdbool.h>
#include <gtk/gtk.h>

typedef struct {
	char	device[32];	/* Serial port device (for terminal). */
	char	path[64];	/* Mountpoint (for upload). */
} AutodetectedTarget;

GSList *	autodetect_all(void);

void		autodetect_free(GSList *list);

#endif		/* AUTODETECT_H_ */
