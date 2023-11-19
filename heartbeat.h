#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#define BLINK_GPIO 13

static const int Output_High = 1;
static const int Output_Low = 0;

int start_heartbeat_task(void);

static const char HEARTBEAT_TAG[] = "HEARTBEAT";

#endif
