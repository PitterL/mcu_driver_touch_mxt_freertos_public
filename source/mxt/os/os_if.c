/*
 * os_if.c
 *
 * Created: 2/27/2016 10:33:32 PM
 *  Author: pitter.liao
 */ 

#include <os_if.h>
#include <os_i2c.h>

extern struct i2c_driver mxt_driver;
extern void dev_debug_init(void);

#if defined(DEBUG)
int mem_used = 0;
#endif

//mem
void *kmalloc(size_t size, unsigned long flags)
{
#if defined(DEBUG)
	mem_used += size;
	//printk("mem %d\n", mem_used);
#endif
	//GFP_KERNEL
	return pvPortMalloc(size);
}

void kfree(void * p)
{
	vPortFree(p);
}

void *kzalloc(size_t size, unsigned long flags)
{
	void * p;
	p = kmalloc(size, flags);
	if (p)
		memset(p, 0, size);
	
	return p;
}

void *kcalloc(size_t num, size_t size, unsigned long flags)
{
	size *= num;

	return kmalloc(size, flags);
}

static void *pa_to_va(void *pa)
{
	return pa;
}

void *dma_alloc_coherent(struct device *dev, size_t size,
		dma_addr_t *dma_handle, unsigned long flags)
{
	*dma_handle = (dma_addr_t)kmalloc(size, flags);

	return pa_to_va((void *)*dma_handle);
}

void dma_free_coherent(struct device *dev, size_t size,
			void *dma_va, dma_addr_t dma_handle)
{
	kfree(dma_va);
}
mutex_p mutex_init(void)
{
	return xSemaphoreCreateMutex();
}

void mutex_destroy(mutex_p lock)
{
	vSemaphoreDelete(lock);
}

void mutex_lock(mutex_p lock)
{
	xSemaphoreTake(lock, TICKS_INFINITE);
}

int mutex_trylock(mutex_p lock)
{
	portBASE_TYPE ret;
	
	ret = xSemaphoreTake(lock, 0);
	if (ret == pdPASS)
		return 0;
	
	return -ETIMEDOUT;
}

void mutex_unlock(mutex_p lock)
{
	xSemaphoreGive(lock);
}

void mutext_destroy(mutex_p lock)
{
	vSemaphoreDelete(lock);
}

completion_p completion_init(void)
{
	xQueueHandle hnd = NULL;

	vSemaphoreCreateBinary(hnd);

	return hnd;
}

void reset_completion(completion_p x)
{
	xSemaphoreTake(x, 0);
}

void complete(completion_p x)
{
	xSemaphoreGive(x);
}

long wait_for_completion_interruptible_timeout(
			completion_p x, jiffies_t timeout)
{
	portBASE_TYPE ret;
	
	ret = xSemaphoreTake(x, timeout);
	if (ret == pdPASS)
		return 0;
	
	return -ETIMEDOUT;
}

void completion_remove(completion_p x)
{
	vSemaphoreDelete(x);
}

wait_queue_head_p init_waitqueue_head(void)
{
	return xQueueCreate(1, sizeof(unsigned long));
}

void remove_waitqueue(wait_queue_head_p *w)
{
	if (*w) {
		vQueueDelete(*w);
		*w = NULL;
	}
}

int wait_event_interruptible_timeout(wait_queue_head_p wq, unsigned long cond, unsigned long timeout)
{
	jiffies_t current, end, interval;
	portBASE_TYPE result;
	unsigned long content;
		
	interval = msecs_to_jiffies(timeout);
	current = jiffies;
	end = current + interval;
	
	do {

		if (time_after(current, end))
			break;
		
		interval = end - current;
		result = xQueueReceive(wq, &content, interval);
		if (result == pdPASS) {
			if (content & cond)
				return 0;
		}

		current = jiffies;		
		//printf("wait %d %d\n",current, end);
	} while (time_before_eq(current, end));

	return -ETIMEDOUT;
}

int wake_up_interruptible(wait_queue_head_p wq, unsigned long event, int isr)
{
	portBASE_TYPE priority_task_token = pdFALSE;
	portBASE_TYPE result;

	if (isr) {
		result = xQueueSendToBackFromISR(wq, &event, &priority_task_token);
		/* NOTE: The actual macro to use to force a context switch from an ISR is
		dependent on the port. This is the correct macro for the Open Watcom DOS
		port. Other ports may require different syntax. Refer to the examples
		provided for the port being used to determine the syntax required. */		
		portEND_SWITCHING_ISR(priority_task_token);
	
	} else
		result = xQueueSendToBack(wq, &event, 0);
	
	if (result == pdPASS) {		
		return 0;
	}
	return -EBUSY; 
}

//kthread_run()
/**
 * kthread_should_stop - should this kthread return now?
 *
 * When someone calls kthread_stop() on your kthread, it will be woken
 * and this will return true.  You should then return, and your return
 * value will be passed through to kthread_stop().
 */
bool kthread_should_stop(struct task_struct *ks)
{
	return test_bit(KTHREAD_SHOULD_STOP, &ks->flags);
}

int kthread_stop(struct task_struct *ks)
{
	if (ks) {
		set_bit(KTHREAD_SHOULD_STOP, &ks->flags);
		if (ks->task) {
			vTaskDelete(ks->task);
			ks->task = NULL;
		}
	}
	
	return 0;
}

void set_current_state(int state)
{

}

#if (configUSE_IDLE_HOOK == 1)
void vApplicationIdleHook( void )
{
	//printk("idle\n");
}
#endif

void sys_platform_init(void)
{
	dev_debug_init();
	
	i2c_bus_init(&mxt_driver, I2C0);
	
	vTaskStartScheduler();
}
