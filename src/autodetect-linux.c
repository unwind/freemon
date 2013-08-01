/*
 * frdm-mntr autodetect module.
*/

#include <string.h>

#include <glib.h>

#include "autodetect.h"

/* ------------------------------------------------------------------- */

static GSList * autodetect_all(void)
{
	gchar	*dmesg_out = NULL;
	gint	dmesg_status;

	if(!g_spawn_command_line_sync("dmesg", &dmesg_out, NULL, &dmesg_status, NULL))
		return NULL;

	GRegex	*re_connect = g_regex_new("usb (\\d+-\\d+\\.\\d+): New USB device found, idVendor=1357.+idProduct=(\\d+)", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL);
	GRegex	*re_acm = g_regex_new("cdc_acm (\\d+-\\d+\\.\\d+):(\\d+\\.\\d+):\\s+([A-Za-z0-9]+):\\s+USB ACM device", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL);
	GRegex	*re_scsi = g_regex_new("scsi(\\d+)\\s*:\\s*usb-storage (\\d+-\\d+\\.\\d+)", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL);
	GRegex	*re_sd = g_regex_new("sd (\\d):0:0:0: \\[([a-z]+)\\]\\s+(\\d+) (\\d+)-byte logical blocks", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL);

	gchar *here = dmesg_out;

	while(true)
	{
		gchar *eol = strchr(here, '\n');
		if(eol != NULL)
			*eol = '\0';

//		printf("line: '%s'\n", here);
		GMatchInfo	*vendor_info = NULL;

		if(g_regex_match(re_connect, here, 0, &vendor_info))
		{
			printf("connect: '%s'\n", g_match_info_fetch(vendor_info, 1));
			g_match_info_free(vendor_info);
		}
		if(g_regex_match(re_scsi, here, 0, &vendor_info))
		{
			printf("scsi: '%s' on '%s'\n", g_match_info_fetch(vendor_info, 1), g_match_info_fetch(vendor_info, 2));
			g_match_info_free(vendor_info);
		}
		if(g_regex_match(re_sd, here, 0, &vendor_info))
		{
			printf("sd: '%s' is '%s'\n", g_match_info_fetch(vendor_info, 1), g_match_info_fetch(vendor_info, 2));
			g_match_info_free(vendor_info);
		}
		if(g_regex_match(re_acm, here, 0, &vendor_info))
		{
			printf("serial: '%s' is '%s'\n", g_match_info_fetch(vendor_info, 1), g_match_info_fetch(vendor_info, 3));
			g_match_info_free(vendor_info);
		}

		if(eol == NULL)
			break;
		here = eol + 1;
	}
	g_regex_unref(re_sd);
	g_regex_unref(re_scsi);
	g_regex_unref(re_acm);
	g_regex_unref(re_connect);
	g_free(dmesg_out);

	return NULL;
}

bool autodetect_simple(GString *device, GString *path)
{
	gchar	*dmesg_out = NULL;
	gint	dmesg_status;

	autodetect_all();
	return false;

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
