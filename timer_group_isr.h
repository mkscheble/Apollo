#ifndef TIMER_GROUP_ISR_H
#define TIMER_GROUP_ISR_H

static const char TIMER_TAG[] = "ISR";
void start_timer_isr_task(void);

static const int cpu_resolution = 1000000; //hz, 1MHz, 1 tick=1us
static const int alarm_ticks = 30000000; // period = 10s


#endif