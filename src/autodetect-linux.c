/*
 * frdm-mntr autodetect module.
*/

#include <string.h>

#include <glib.h>

#include "autodetect.h"

/* ------------------------------------------------------------------- */

typedef struct {
	char	usb[16];
	char	serial[16];
	char	disk[8];
} DeviceTmp;

typedef struct {
	GRegex	*re_connect;
	GRegex	*re_acm;
	GRegex	*re_scsi;
	GRegex	*re_sd;
	GRegex	*re_disconnect;
	char	usb[16];
	char	scsi[8];
	char	disk[8];
	char	serial[16];
	GList	*devices;	/* List of DeviceTmp. */
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

void devicetmp_append(AutodetectInfo *info)
{
	DeviceTmp	*tmp = g_malloc(sizeof *tmp);

	g_strlcpy(tmp->usb, info->usb, sizeof tmp->usb);
	g_strlcpy(tmp->serial, info->serial, sizeof tmp->serial);
	g_strlcpy(tmp->disk, info->disk, sizeof tmp->disk);

	info->devices = g_list_append(info->devices, tmp);
	printf("added '%s', now %u\n", info->usb, g_list_length(info->devices));
}

void devicetmp_delete(AutodetectInfo *info, const char *usb)
{
	GList	*iter;

	for(iter = info->devices; iter != NULL; iter = g_list_next(iter))
	{
		DeviceTmp *here = iter->data;

		if(strcmp(here->usb, usb) == 0)
		{
			info->devices = g_list_delete_link(info->devices, iter);
			printf("deleted '%s', now %u\n", usb, g_list_length(info->devices));
			g_free(here);
			return;
		}
	}
}

static bool inspect_line(const char *line, void *user)
{
	AutodetectInfo	*info = user;
	GMatchInfo *match = NULL;

	/* Check for disconnect. */
	if(g_regex_match(info->re_disconnect, line, 0, &match))
	{
		gchar *usb = g_match_info_fetch(match, 1);

		printf("disconnect: '%s'\n", usb);
		devicetmp_delete(info, usb);
		g_free(usb);
		g_match_info_free(match);

		return true;
	}

	/* Not a disconnect, collect info for new device. */
	if(g_regex_match(info->re_connect, line, 0, &match))
	{
		gchar *usb = g_match_info_fetch(match, 1);
		printf("connect: '%s'\n", usb);
		g_strlcpy(info->usb, usb, sizeof info->usb);
		g_free(usb);
	}
	else if(info->usb[0] != '\0' && g_regex_match(info->re_acm, line, 0, &match))
	{
		gchar *usb = g_match_info_fetch(match, 1), *serial = g_match_info_fetch(match, 3);

		if(strcmp(usb, info->usb) == 0)
		{
			g_strlcpy(info->serial, serial, sizeof info->serial);
			printf("serial device: '%s'\n", info->serial);
		}
		g_free(serial);
		g_free(usb);
	}
	else if(info->usb[0] != '\0' && g_regex_match(info->re_scsi, line, 0, &match))
	{
		gchar *scsi = g_match_info_fetch(match, 1), *usb = g_match_info_fetch(match, 2);

		if(strcmp(usb, info->usb) == 0)
		{
			g_strlcpy(info->scsi, scsi, sizeof info->scsi);

			printf("scsi: '%s' on '%s'\n", scsi, usb);
		}
		g_free(usb);
		g_free(scsi);
	}
	if(info->scsi[0] != '\0' && g_regex_match(info->re_sd, line, 0, &match))
	{
		gchar *scsi = g_match_info_fetch(match, 1), *disk = g_match_info_fetch(match, 2);

		if(strcmp(scsi, info->scsi) == 0)
		{
			g_strlcpy(info->disk, disk, sizeof info->disk);
			printf("scsi disk: '%s'\n", info->disk);
		}
		g_free(disk);
		g_free(scsi);
	}
	g_match_info_free(match);

	/* Now figure out if we have all we need. */
	if(info->usb[0] != '\0' && info->disk[0] != '\0' && info->serial[0] != '\0')
	{
		devicetmp_append(info);
		info->usb[0] = info->scsi[0] = info->disk[0] = info->serial[0] = '\0';
	}
	return true;
}

static GSList * autodetect_all(void)
{
	gchar	*dmesg_out = NULL;
	gint	dmesg_status;

	if(!g_spawn_command_line_sync("dmesg", &dmesg_out, NULL, &dmesg_status, NULL))
		return NULL;

	AutodetectInfo info = {
		.re_connect = g_regex_new("usb (\\d+-\\d+\\.\\d+): New USB device found, idVendor=1357.+idProduct=(\\d+)", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL),
		.re_acm = g_regex_new("cdc_acm (\\d+-\\d+\\.\\d+):(\\d+\\.\\d+):\\s+([A-Za-z0-9]+):\\s+USB ACM device", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL),
		.re_scsi = g_regex_new("scsi(\\d+)\\s*:\\s*usb-storage (\\d+-\\d+\\.\\d+)", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL),
		.re_sd = g_regex_new("sd (\\d):0:0:0: \\[([a-z]+)\\]\\s+(\\d+) (\\d+)-byte logical blocks", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL),
		.re_disconnect = g_regex_new("usb (\\d+-\\d+\\.\\d+): USB disconnect, device number (\\d+)", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL),
		.usb = "",
		.scsi = "",
		.disk = "",
		.serial = "",
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
