/*
 * irqreturn.h
 *
 * Created: 3/2/2016 2:15:48 PM
 *  Author: pitter.liao
 */ 


#ifndef IRQRETURN_H_
#define IRQRETURN_H_

/**
 * enum irqreturn
 * @IRQ_NONE		interrupt was not from this device
 * @IRQ_HANDLED		interrupt was handled by this device
 * @IRQ_WAKE_THREAD	handler requests to wake the handler thread
 */
enum irqreturn {
	IRQ_NONE		= (0 << 0),
	IRQ_HANDLED		= (1 << 0),
	IRQ_WAKE_THREAD		= (1 << 1),
};

typedef enum irqreturn irqreturn_t;
#define IRQ_RETVAL(x)	((x) != IRQ_NONE)



#endif /* IRQRETURN_H_ */