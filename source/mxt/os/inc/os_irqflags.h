/*
 * os_irqflags.h
 *
 * Created: 3/11/2016 6:09:45 PM
 *  Author: pitter.liao
 */ 


#ifndef OS_IRQFLAGS_H_
#define OS_IRQFLAGS_H_

#define raw_local_irq_save(flags)			\
do {						\
	flags = cpu_irq_save();		\
} while (0)
#define raw_local_irq_restore(flags)			\
do {						\
	cpu_irq_restore(flags);		\
} while (0)

#endif /* OS_IRQFLAGS_H_ */