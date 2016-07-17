/*
 * mutex.h
 *
 * Created: 3/12/2016 9:12:40 AM
 *  Author: pitter.liao
 */ 


#ifndef MUTEX_H_
#define MUTEX_H_

typedef void * mutex_p;

mutex_p mutex_init(void);
void mutex_destroy(mutex_p);
void mutex_lock(mutex_p);
int mutex_trylock(mutex_p);
void mutex_unlock(mutex_p);
void mutext_destroy(mutex_p);

#endif /* MUTEX_H_ */