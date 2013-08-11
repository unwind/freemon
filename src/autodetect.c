/*
 * frdm-mntr autodetect module.
*/

#if defined __linux
#include "autodetect-linux.c"
#else
#error "Missing autodetect implementation for this platform, sorry!"
#endif

void autodetect_free(GSList *list)
{
	for(GSList *iter = list; iter != NULL; iter = g_slist_next(iter))
		g_free(iter->data);
	g_slist_free(list);
}
