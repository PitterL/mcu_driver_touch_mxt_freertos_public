/*
 * debug.h
 *
 * Created: 2/27/2016 11:27:52 PM
 *  Author: pitter.liao
 */ 


#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdarg.h>
#include <stdio.h>
#include "device.h"

#ifndef pr_fmt
#define pr_fmt(fmt) fmt
#endif

#ifndef printk
#define printk printf
#endif

#define KERN_SOH	"\001"		/* ASCII Start Of Header */
#define KERN_SOH_ASCII	'\001'

#define KERN_EMERG	KERN_SOH "0"	/* system is unusable */
#define KERN_ALERT	KERN_SOH "1"	/* action must be taken immediately */
#define KERN_CRIT	KERN_SOH "2"	/* critical conditions */
#define KERN_ERR	KERN_SOH "3"	/* error conditions */
#define KERN_WARNING	KERN_SOH "4"	/* warning conditions */
#define KERN_NOTICE	KERN_SOH "5"	/* normal but significant condition */
#define KERN_INFO	KERN_SOH "6"	/* informational */
#define KERN_DEBUG	KERN_SOH "7"	/* debug-level messages */

struct va_format {
	const char *fmt;
	va_list *va;
};

enum {
	DUMP_PREFIX_NONE,
	DUMP_PREFIX_ADDRESS,
	DUMP_PREFIX_OFFSET
};

static inline void print_hex_dump(const char *level, const char *prefix_str,
									int prefix_type, int rowsize, int groupsize,
									const void *buf, size_t len, bool ascii)
{
	int i, j, row;
	
	if (groupsize == 2) {
		len >>= 1;
	} else if (groupsize == 4) {
		len >>= 2;
	}
	
	if (rowsize == 0)
		rowsize = 1;
	row = (len + rowsize -1) / rowsize;
	
	printk("%s: ", prefix_str);
	
	for (i = 0; i < row; i++) {
		for (j = 0; j < rowsize; j++) {
			if (groupsize == 2) {
				printk("%04x ", *((unsigned short *)buf + i * rowsize + j));
			}else if (groupsize == 4) {
				printk("%08x ", *((unsigned int *)buf + i * rowsize + j));
			}else {
				printk("%02x ", *((unsigned char *)buf + i * rowsize + j));
			}
		}
	}
	
	printk("\n");
}


static inline size_t dec_dump_to_buffer(const char *prefix, const void *buf, size_t num, int groupsize,
	char *linebuf, size_t linebuflen)
{
	int j, lx = 0;

	if (prefix)
		lx += snprintf(linebuf, linebuflen - lx, "%s: ", prefix);

	if (groupsize == sizeof(s16))
		for (j = 0; j < num; j++)
			lx += snprintf(linebuf + lx, linebuflen - lx,
				"%s%7d", j ? "," : "", (int)*((s16 *)buf + j));
	else if(groupsize == sizeof(s32))
		for (j = 0; j < num; j++)
			lx += snprintf(linebuf + lx, linebuflen - lx,
				"%s%12d", j ? "," : "", (int)*((s32 *)buf + j));
	else
		for (j = 0; j < num; j++)
			lx += snprintf(linebuf + lx, linebuflen - lx,
				"%s%5d", j ? "," : "", (int)*((s8 *)buf + j));

	if (lx == linebuflen)
		lx--;

	linebuf[lx++] = '\n';
	linebuf[lx++] = '\0';

	return lx;
}

static inline void print_dec_matrix(const char *level,const char *prefix_str,const void *buf,int x_size,int y_size)
{
	const s16 *ptr = buf;
	int i;
	char linebuf[7 * y_size + 1];

	for (i = 0; i < x_size; i ++) {
		dec_dump_to_buffer(NULL, ptr + i * y_size, y_size, 2,
			linebuf, sizeof(linebuf));
		printk("%s %s\t%3d:  %s\n", level, prefix_str, i, linebuf);
	}
}

static inline void print_dec16_buf(const char *level,const char *prefix_str,const s16 *buf,int num)
{
	const s16 *ptr = buf;
	char linebuf[8 * num + 1];

	dec_dump_to_buffer(NULL, ptr, num, 2,
		linebuf, sizeof(linebuf));
	printk("%s %10s:  %s", level, prefix_str, linebuf);
}

static inline void print_matrix(const char *prefix,const s16 *buf,int x_size,int y_size)
{
	if (prefix)
		printk(KERN_INFO "Print matrix %s :\n",prefix);
	else
		printk(KERN_INFO "Print address %p :\n",buf);

	print_dec_matrix(KERN_INFO,"    ",buf,x_size,y_size);
}

extern int dev_printk(const char *level, const struct device *dev,
	const char *fmt, ...) __attribute__ ((format (printf, 3, 0)));

#define dev_dbg(dev, format, arg...)		\
	dev_printk(KERN_DEBUG, dev, format, ##arg)

#define dev_info(dev, format, arg...)		\
	dev_printk(KERN_INFO, dev, format, ##arg)

#define dev_warn(dev, format, arg...)		\
	dev_printk(KERN_WARNING, dev, format, ##arg)

#define dev_err(dev, format, arg...)		\
	dev_printk(KERN_ERR, dev, format, ##arg)

#define __WARN() printk("WARNING Line %d function %s\n",__LINE__, __FUNCTION__)

#define __WARN_printf(fmt, ...)			\
({						\
	__WARN();	\
	printk_once(fmt);					\
})

/**
 * \def likely(exp)
 * \brief The expression \a exp is likely to be true
 */
#if !defined(likely) || defined(__DOXYGEN__)
#   define likely(exp)    (exp)
#endif

/**
 * \def unlikely(exp)
 * \brief The expression \a exp is unlikely to be true
 */
#if !defined(unlikely) || defined(__DOXYGEN__)
#   define unlikely(exp)  (exp)
#endif

#ifndef WARN_ON
#define WARN_ON(condition) ({						\
	int __ret_warn_on = !!(condition);				\
	if (unlikely(__ret_warn_on))					\
	__WARN();						\
	unlikely(__ret_warn_on);					\
})
#endif

#ifndef WARN
#define WARN(condition, format...) ({						\
	int __ret_warn_on = !!(condition);				\
	if (unlikely(__ret_warn_on))					\
	__WARN_printf(format);					\
	unlikely(__ret_warn_on);					\
})
#endif

#define printk_once(fmt, ...)			\
({						\
	static bool __print_once;		\
	\
	if (!__print_once) {			\
		__print_once = true;		\
		printk(fmt, ##__VA_ARGS__);	\
	}					\
})

#define pr_emerg(fmt, ...)					\
printk_once(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__)
#define pr_alert(fmt, ...)					\
printk_once(KERN_ALERT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_crit(fmt, ...)					\
printk_once(KERN_CRIT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_err(fmt, ...)					\
printk_once(KERN_ERR pr_fmt(fmt), ##__VA_ARGS__)
#define pr_warn(fmt, ...)					\
printk_once(KERN_WARNING pr_fmt(fmt), ##__VA_ARGS__)
#define pr_notice(fmt, ...)				\
printk_once(KERN_NOTICE pr_fmt(fmt), ##__VA_ARGS__)
#define pr_info(fmt, ...)					\
printk_once(KERN_INFO pr_fmt(fmt), ##__VA_ARGS__)
#define pr_debug(fmt, ...)					\
printk_once(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)

#endif /* DEBUG_H_ */
