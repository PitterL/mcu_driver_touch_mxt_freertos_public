/*
 * pq.h
 *
 * Created: 3/12/2016 9:24:02 AM
 *  Author: pitter.liao
 */ 


#ifndef PQ_H_
#define PQ_H_

#include "os_if.h"

//ticks
typedef portTickType TickType_t;
typedef volatile TickType_t jiffies_t;
#if( configUSE_16_BIT_TICKS == 1 )
typedef int16_t s_jiffies_t;
#define MAX_SCHEDULE_TIMEOUT SHRT_MAX
#else
typedef int32_t s_jiffies_t;
#define MAX_SCHEDULE_TIMEOUT INT_MAX
#endif

#define TICKS_INFINITE portMAX_DELAY
#define HZ configTICK_RATE_HZ
#define MS portTICK_RATE_MS
#define US_CPU_CLOCK (configCPU_CLOCK_HZ / 1000 / 1000)

#define jiffies raid6_jiffies()
static inline jiffies_t raid6_jiffies(void){
	return xTaskGetTickCount();
}

static inline unsigned int jiffies_to_msecs(const jiffies_t j)
{
	return j / MS;
}

static inline jiffies_t msecs_to_jiffies(const unsigned int m)
{
	return m * MS;
}

static inline void msleep(unsigned int msecs)
{
	jiffies_t ticks = msecs_to_jiffies(msecs);
	
	vTaskDelay(ticks);
	//delay_ms(msecs);
}

extern uint32_t SystemCoreClock;
static inline void usleep(unsigned long usecs)
{
	volatile unsigned long loop_count = usecs * SystemCoreClock;
	
	while(loop_count--) __asm("NOP");
	//delay_us(usecs);
}

static inline void udelay(unsigned long usecs)
{
	usleep(usecs);
}

#endif /* PQ_H_ */
