/*
 * slab.h
 *
 * Created: 3/12/2016 8:43:04 AM
 *  Author: pitter.liao
 */ 


#ifndef SLAB_H_
#define SLAB_H_

#define GFP_KERNEL 0x1

//memory
extern void *kmalloc(size_t, unsigned long);
extern void *kcalloc(size_t, size_t, unsigned long);
extern void kfree(void *);
extern void *kzalloc(size_t size, unsigned long flags);

#endif /* SLAB_H_ */