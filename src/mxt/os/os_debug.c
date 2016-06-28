/*
 * os_debug.c
 *
 * Created: 3/23/2016 4:23:57 PM
 *  Author: pitter.liao
 */ 
#include <errno.h>
#include <string.h>
#include <status_codes.h>

#include <common/types.h>
#include <common/slab.h>
#include <common/debug.h>

void print_trunk(const u8 *data, int pos, int len)
{
	//#define CONFIG_MXT_TRACK_TRUNK_DATA 1024
#if defined(CONFIG_MXT_TRACK_TRUNK_DATA)
	u8 *buf = kmalloc(CONFIG_MXT_TRACK_TRUNK_DATA, GFP_KERNEL);
	if (buf == NULL)
	return;

	if (!len) {
		printk(KERN_INFO "[mxt] error len pos %d offset %d\n", pos, len);
		return;
	}

	if (len + 1 > CONFIG_MXT_TRACK_TRUNK_DATA) {
		printk(KERN_INFO "[mxt] too long string: pos %d offset %d\n", pos, len);
		len = sizeof(buf) - 1;
	}
	memcpy(buf, data + pos, len);
	buf[len] = '\0';
	printk(KERN_INFO "[mxt] %s \n",buf);
	kfree(buf);
#endif
}

#define DEBUG_LEVEL KERN_INFO
char debug_buffer[128];
static int __dev_printk(const char *level, const struct device *dev,
struct va_format *vaf)
{
	if (level) {
		if (strcmp(DEBUG_LEVEL, level) >= 0) {
			vsnprintf(debug_buffer, sizeof(debug_buffer), vaf->fmt, *vaf->va);
			puts(debug_buffer);
		}
	}

	return 0;
}

int dev_printk(const char *level, const struct device *dev,
const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;
	int r;

	va_start(args, fmt);

	vaf.fmt = fmt;
	vaf.va = &args;

	r = __dev_printk(level, dev, &vaf);

	va_end(args);

	return r;
}
