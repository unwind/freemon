/*
 * frdm-mntr autodetect module.
*/

#include <string.h>

#include <glib.h>

#include "autodetect.h"

/* ------------------------------------------------------------------- */

typedef enum {
	CONNECT = 0,
	ACM,
	SCSI,
	SD
} State;

typedef struct {
	State	state;
	char	usb[16];
	GRegex	*re_connect;
	GRegex	*re_acm;
	GRegex	*re_scsi;
	GRegex	*re_sd;
	GRegex	*re_disconnect;
} AutodetectInfo;

/* ------------------------------------------------------------------- */

/* FIXME: Move this into a standalone module? */
static void split_foreach(char *string, bool (*callback)(const char *line, void *user), void *user)
{
	while(true)
	{
		gchar *eol = strchr(string, '\n');
		if(eol != NULL)
			*eol = '\0';
		if(!callback(string, user))
			break;

		if(eol == NULL)
			break;
		string = eol + 1;
	}
}

static bool inspect_line(const char *line, void *user)
{
	AutodetectInfo	*info = user;
	GMatchInfo *match = NULL;

	if(g_regex_match(info->re_connect, line, 0, &match))
	{
		printf("connect: '%s'\n", g_match_info_fetch(match, 1));
	}
	else if(g_regex_match(info->re_scsi, line, 0, &match))
	{
		printf("scsi: '%s' on '%s'\n", g_match_info_fetch(match, 1), g_match_info_fetch(match, 2));
	}
	else if(g_regex_match(info->re_sd, line, 0, &match))
	{
		printf("sd: '%s' is '%s'\n", g_match_info_fetch(match, 1), g_match_info_fetch(match, 2));
	}
	else if(g_regex_match(info->re_acm, line, 0, &match))
	{
		printf("serial: '%s' is '%s'\n", g_match_info_fetch(match, 1), g_match_info_fetch(match, 3));
	}
	else if(g_regex_match(info->re_disconnect, line, 0, &match))
	{
		printf("disconnect: '%s' (device '%s')\n", g_match_info_fetch(match, 1), g_match_info_fetch(match, 2));
	}
	g_match_info_free(match);

	return true;
}

static GSList * autodetect_all(void)
{
	gchar	*dmesg_out = NULL;
	gint	dmesg_status;

	if(!g_spawn_command_line_sync("dmesg", &dmesg_out, NULL, &dmesg_status, NULL))
		return NULL;

	AutodetectInfo info = {
		.state = CONNECT,
		.re_connect = g_regex_new("usb (\\d+-\\d+\\.\\d+): New USB device found, idVendor=1357.+idProduct=(\\d+)", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL),
		.re_acm = g_regex_new("cdc_acm (\\d+-\\d+\\.\\d+):(\\d+\\.\\d+):\\s+([A-Za-z0-9]+):\\s+USB ACM device", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL),
		.re_scsi = g_regex_new("scsi(\\d+)\\s*:\\s*usb-storage (\\d+-\\d+\\.\\d+)", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL),
		.re_sd = g_regex_new("sd (\\d):0:0:0: \\[([a-z]+)\\]\\s+(\\d+) (\\d+)-byte logical blocks", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL),
		.re_disconnect = g_regex_new("usb (\\d+-\\d+\\.\\d+): USB disconnect, device number (\\d+)", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL),
	};

	split_foreach(dmesg_out, inspect_line, &info);

	g_regex_unref(info.re_disconnect);
	g_regex_unref(info.re_sd);
	g_regex_unref(info.re_scsi);
	g_regex_unref(info.re_acm);
	g_regex_unref(info.re_connect);
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
