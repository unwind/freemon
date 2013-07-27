/*
 * frdm-mntr autodetect module.
*/

#include <glib.h>

#include "autodetect.h"

/* ------------------------------------------------------------------- */

bool autodetect_simple(GString *device, GString *path)
{
	gchar	*dmesg_out = NULL;
	gint	dmesg_status;

	if(g_spawn_command_line_sync("dmesg", &dmesg_out, NULL, &dmesg_status, NULL))
	{
		GRegex	*vendor_re = g_regex_new("usb (\\d+-\\d+\\.\\d+): .+ idVendor=1357.+idProduct=(\\d+)", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL);
		
		if(vendor_re != NULL)
		{
			GMatchInfo	*vendor_info = NULL;

			if(g_regex_match(vendor_re, dmesg_out, 0, &vendor_info))
			{
				printf("got %zu bytes, status=%d\n", strlen(dmesg_out), dmesg_status);
				printf("match: '%s'\n", g_match_info_fetch(vendor_info, 1));
				g_match_info_free(vendor_info);
			}
			else
				printf("no match\n");
			g_regex_unref(vendor_re);
		}
		g_free(dmesg_out);
	}
	else
		printf("that's a fail\n");

	return false;
}
