/*
 * frdm-mntr autodetect module.
*/

#include <ctype.h>
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

	GRegex	*re_mount;
	char	usb[16];
	char	scsi[8];
	char	disk[8];
	char	serial[16];
	GList	*tmp_devices;	/* List of DeviceTmp. */
	GSList	*targets;
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

	info->tmp_devices = g_list_append(info->tmp_devices, tmp);
	printf("added '%s', now %u\n", info->usb, g_list_length(info->tmp_devices));
}

void devicetmp_delete(AutodetectInfo *info, const char *usb)
{
	GList	*iter;

	for(iter = info->tmp_devices; iter != NULL; iter = g_list_next(iter))
	{
		DeviceTmp *here = iter->data;

		if(strcmp(here->usb, usb) == 0)
		{
			info->tmp_devices = g_list_delete_link(info->tmp_devices, iter);
			printf("deleted '%s', now %u\n", usb, g_list_length(info->tmp_devices));
			g_free(here);
			return;
		}
	}
}

static bool dmesg_inspect_line(const char *line, void *user)
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
	else if(info->scsi[0] != '\0' && g_regex_match(info->re_sd, line, 0, &match))
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

static bool df_inspect_line(const char *line, void *user)
{
	AutodetectInfo	*info = user;
	GList		*iter;

	for(iter = info->tmp_devices; iter != NULL; iter = g_list_next(iter))
	{
		if(line[0] == '/')	/* Quick check for mountpoint. */
		{
			const DeviceTmp *tmp = iter->data;
			gchar dev[64];
			const gsize len = g_snprintf(dev, sizeof dev, "/dev/%s", tmp->disk);
			if(strncmp(line, dev, len) == 0 && isspace((unsigned int) line[len]))
			{
				/* Found device in mounted device list, extract mountpoint. */
				const char *sep = strchr(line + len, '/');
				if(sep != NULL)
				{
					/* Allocate and append target descriptor. */
					AutodetectedTarget *target = g_malloc(sizeof *target);
					g_snprintf(target->device, sizeof target->device, "/dev/%s", tmp->serial);
					g_strlcpy(target->path, sep, sizeof target->path);
					info->targets = g_slist_append(info->targets, target);
				}
			}
		}
	}
	return true;
}

GSList * autodetect_all(void)
{
	gchar	*dmesg_out = NULL;
	gint	dmesg_status;

	if(!g_spawn_command_line_sync("dmesg", &dmesg_out, NULL, &dmesg_status, NULL))
		return NULL;

#define	RE_USB	"(\\d+-(?:\\d\\.?)+)"

	AutodetectInfo info = {
		.re_connect = g_regex_new("usb " RE_USB ".*: new usb device found, idvendor=1357.+idproduct=(\\d+)", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL),
		.re_acm = g_regex_new("cdc_acm " RE_USB ":(\\d+\\.\\d+):\\s+([A-Za-z0-9]+):\\s+usb acm device", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL),
		.re_scsi = g_regex_new("scsi(\\d+)\\s*:\\s*usb-storage " RE_USB, G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL),
		.re_sd = g_regex_new("sd (\\d+):0:0:0: \\[([a-z]+)\\]\\s+(\\d+) (\\d+)-byte logical blocks", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL),
		.re_disconnect = g_regex_new("usb " RE_USB ": usb disconnect, device number (\\d+)", G_REGEX_CASELESS | G_REGEX_OPTIMIZE, 0, NULL),
		.usb = "",
		.scsi = "",
		.disk = "",
		.serial = "",
		.tmp_devices = NULL,
		.targets = NULL,
	};

	split_foreach(dmesg_out, dmesg_inspect_line, &info);

	g_regex_unref(info.re_disconnect);
	g_regex_unref(info.re_sd);
	g_regex_unref(info.re_scsi);
	g_regex_unref(info.re_acm);
	g_regex_unref(info.re_connect);
	g_free(dmesg_out);

	/* Any devices found that we need to resolve against mounted filesystems? */
	if(info.tmp_devices != NULL)
	{
		gchar	*df_out = NULL;
		gint	df_status;

		if(!g_spawn_command_line_sync("df --local --portability", &df_out, NULL, &df_status, NULL))
			return NULL;

		split_foreach(df_out, df_inspect_line, &info);
		g_list_free_full(info.tmp_devices, g_free);
	}
	printf("found %u targets\n", g_slist_length(info.targets));
	return info.targets;
}
