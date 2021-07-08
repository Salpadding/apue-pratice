#ifndef ANY_TIMER_H__
#define ANY_TIMER_H__

typedef void (*at_job)(void *);
typedef int at_id;

at_id at_add_job(int sec, at_job job, const void * arg);

int at_cancel_job(at_id id);

void at_wait(at_id id);

#endif