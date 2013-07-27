/*
 * frdm-mntr auto-detect module.
*/

#if !defined AUTODETECT_H_
#define	AUTODETECT_H_

#include <stdbool.h>
#include <gtk/gtk.h>

bool	autodetect_simple(GString *device, GString *path);

#endif		/* AUTODETECT_H_ */
