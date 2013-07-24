/*
 * frdm-mntr actions module.
*/

#if !defined ACTIONS_H_
#define	ACTIONS_H_

#include <gtk/gtk.h>

typedef struct {
	GtkAction	*connect;
	GtkAction	*upload;
} Actions;

#endif		/* ACTIONS_H_ */
