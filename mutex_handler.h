#ifndef MUTEX_HANDLER_H
#define MUTEX_HANDLER_H

#include "freertos/semphr.h"

typedef struct {
    bool permission_granted;
} example_queue_element_t;

extern SemaphoreHandle_t xMutexPermission;
extern SemaphoreHandle_t xMutexHttpData;

void vCreateMutex(void);
void vhttpPermission(example_queue_element_t param);

#endif