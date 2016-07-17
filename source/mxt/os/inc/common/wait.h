/*
 * wait.h
 *
 * Created: 3/12/2016 11:30:19 AM
 *  Author: pitter.liao
 */ 


#ifndef WAIT_H_
#define WAIT_H_


typedef void * wait_queue_head_p;

wait_queue_head_p init_waitqueue_head(void);
void remove_waitqueue(wait_queue_head_p *w);
int wake_up_interruptible(wait_queue_head_p, unsigned long, int);
int wait_event_interruptible_timeout(wait_queue_head_p, unsigned long, unsigned long);

#endif /* WAIT_H_ */