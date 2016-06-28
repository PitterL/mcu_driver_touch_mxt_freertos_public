/*
 * os_if.c
 *
 * Created: 2/27/2016 10:33:32 PM
 *  Author: pitter.liao
 */ 

#include <os_if.h>

//mem
void *kmalloc(size_t size, unsigned long flags)
{
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
	p = pvPortMalloc(size);
	if (p)
		memset(p, 0, size);
	
	return p;
}

void *kcalloc(size_t num, size_t size, unsigned long flags)
{
	size *= num;
	return pvPortMalloc(size);
}

static void *pa_to_va(void *pa)
{
	return pa;
}

void *dma_alloc_coherent(struct device *dev, size_t size,
		dma_addr_t *dma_handle, unsigned long flags)
{
	*dma_handle = kmalloc(size, flags);

	return pa_to_va(*dma_handle);
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
	task_handle_p task = NULL;
	if (ks) {
		task = ks->task;
		set_bit(KTHREAD_SHOULD_STOP, &ks->flags);
	}
	
	vTaskDelete(task);
	
	return 0;
}

void set_current_state(int state)
{

}

#include <extint.h>
#include <io/l21/asf_wrapper/hal_ext_irq.h>
#include <status_codes.h>

#define EIC_TEST_CHANNEL      5
#define EIC_TEST_PIN          PIN_PA21A_EIC_EXTINT5
#define EIC_TEST_PIN_MUX      PINMUX_PA21A_EIC_EXTINT5
int32_t ext_irq_register(const uint32_t irq, ext_irq_cb_t cb)
{
	struct extint_chan_conf eic_conf;
	enum status_code result;

	WARN_ON(irq != EIC_TEST_CHANNEL);

	/* Configure the external interrupt channel */
	extint_chan_get_config_defaults(&eic_conf);
	eic_conf.gpio_pin           = EIC_TEST_PIN;
	eic_conf.gpio_pin_mux       = EIC_TEST_PIN_MUX;
	eic_conf.gpio_pin_pull      = EXTINT_PULL_UP;
	eic_conf.detection_criteria = EXTINT_DETECT_LOW;
	extint_chan_set_config(EIC_TEST_CHANNEL, &eic_conf);
	/* Register and enable the callback function */
	result = extint_register_callback(cb,
		EIC_TEST_CHANNEL,EXTINT_CALLBACK_TYPE_DETECT);
	
	extint_chan_enable_callback(EIC_TEST_CHANNEL,
		EXTINT_CALLBACK_TYPE_DETECT);

	if (result == STATUS_OK)
		return 0;

	return -EIO;
}

int32_t ext_irq_enable(const uint32_t irq)
{
	extint_chan_enable_callback(EIC_TEST_CHANNEL,
		EXTINT_CALLBACK_TYPE_DETECT);
    
	return 0;
}

int32_t ext_irq_disable(const uint32_t irq)
{
	extint_chan_disable_callback(EIC_TEST_CHANNEL,
		EXTINT_CALLBACK_TYPE_DETECT);
	
	return 0;
}
#if (configUSE_IDLE_HOOK == 1)
void vApplicationIdleHook( void )
{
	//printk("idle\n");
}
#endif
