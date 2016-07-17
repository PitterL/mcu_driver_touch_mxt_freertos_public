/*
 * os_input.c
 *
 * Created: 3/18/2016 3:14:42 PM
 *  Author: pitter.liao
 */ 
#include <errno.h>
#include <string.h>
#include <stdlib.h>
//#include <math.h>

#include <common/types.h>
#include <common/slab.h>
#include <common/debug.h>
#include <common/input.h>

#include <os_if.h>

void input_event_hook(int type, unsigned int code, int value)
{
	/*
	if (code == ABS_MT_ORIENTATION)
		vec_to_angle(value);

	if (code == ABS_MT_POSITION_X)
		printk("x = %d ", value);
	else if (code == ABS_MT_POSITION_Y)
		printk("y = %d\n", value);
	*/
}
