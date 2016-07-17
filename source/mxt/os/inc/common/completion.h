/*
 * completion.h
 *
 * Created: 3/12/2016 11:22:50 AM
 *  Author: pitter.liao
 */ 


#ifndef COMPLETION_H_
#define COMPLETION_H_


typedef void * completion_p;
completion_p completion_init(void);
void reset_completion(completion_p);
void complete(completion_p);
long wait_for_completion_interruptible_timeout(completion_p, unsigned long);
void completion_remove(completion_p);

#endif /* COMPLETION_H_ */