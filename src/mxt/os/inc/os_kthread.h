/*
 * kthread.h
 *
 * Created: 3/12/2016 10:55:05 AM
 *  Author: pitter.liao
 */ 


#ifndef KTHREAD_H_
#define KTHREAD_H_

#include "common/kernel.h"
#include "common/sched.h"

typedef xTaskHandle TaskHandle_t;
typedef TaskHandle_t task_handle_p;
struct task_struct{
	task_handle_p task;
	unsigned long flags;
};

/**
 * kthread_run - create and wake a thread.
 * @threadfn: the function to run until signal_pending(current).
 * @data: data ptr for @threadfn.
 * @namefmt: printf-style name for the thread.
 *
 * Description: Convenient wrapper for kthread_create() followed by
 * wake_up_process().  Returns the kthread or ERR_PTR(-ENOMEM).
 */
#define DEFAULT_TASK_STACK_DEPTH 1000
#define DEFAULT_TASK_PRIORITY (configTIMER_TASK_PRIORITY + 1)
#define kthread_run(__threadfn, __data, __name)			   \
({									   \
	task_handle_p __k;						   \
	portBASE_TYPE __result		\
		= xTaskCreate(__threadfn, __name, DEFAULT_TASK_STACK_DEPTH, __data, DEFAULT_TASK_PRIORITY, &__k); \
	if (__result != pdTRUE)						   \
		__k = NULL;					   \
	__k;								   \
})

#define __to_task_struct(hnd)	\
	container_of(hnd, struct task_struct, task)

static inline struct task_struct *to_task_struct(struct task_struct *ks)
{
	return __to_task_struct(ks->task);
}

bool kthread_should_stop(struct task_struct *);
int kthread_stop(struct task_struct *);

#endif /* KTHREAD_H_ */