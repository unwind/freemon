/*
 * frdm-mntr auto-detect module.
*/

#if !defined AUTODETECT_H_
#define	AUTODETECT_H_

#include <stdbool.h>
#include <gtk/gtk.h>

typedef struct {
	char	device[32];
	char	path[64];
} AutodetectedTarget;

bool	autodetect_simple(GString *device, GString *path);

#endif		/* AUTODETECT_H_ */
