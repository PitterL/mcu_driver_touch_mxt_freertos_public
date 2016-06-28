/*
 * os_if.h
 *
 * Created: 3/12/2016 9:33:26 AM
 *  Author: pitter.liao
 */ 


#ifndef OS_IF_H_
#define OS_IF_H_

#include <FreeRTOS.h>
#include <portmacro.h>
#include <task.h>
#include <semphr.h>
#include <string.h>
#include <delay.h>
#include <errno.h>

#include <common/types.h>

//dma
#include "os_dma_mapping.h"
//irq
#include "os_irqflags.h"
//thread
#include "os_kthread.h"
//ticks
#include "os_pq.h"

//slab
#include "common/slab.h"
//mutex
#include "common/mutex.h"
//completion
#include "common/completion.h"
//wait
#include "common/wait.h"
//jiffies
#include "common/jiffies.h"
//test flag op
#include "common/atomic_op.h"
//test bit op
#include "common/bitops.h"
//debug
#include "common/debug.h"

#define MODULE_AUTHOR(_author)
#define MODULE_DESCRIPTION(_desc);
#define MODULE_LICENSE(_license);

#endif /* OS_IF_H_ */