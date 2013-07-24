/*
 * frdm-mntr target module.
*/

#if !defined TARGET_H_
#define	TARGET_H_

#include <gtk/gtk.h>

#include "actions.h"

typedef struct Target	Target;

Target *	target_new_serial(const char *name, const char *tty_device, const char *upload_path);
void		target_destroy(Target *target);

const char *	target_get_name(const Target *target);
const char *	target_get_device(const Target *target);
const char *	target_get_directory(const Target *target);

void		target_gui_terminal_set_keyhandler(Target *target, void (*handler)(guint32 unicode, gpointer user), gpointer user);
void		target_gui_terminal_insert(Target *target, const char *text, size_t length);

GtkWidget *	target_gui_create(Target *target, const Actions *actions);
void		target_gui_destroy(Target *target);

#endif		/* TARGET_H_ */
