#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define ENCODER_1 GPIO_NUM_4
#define ENCODER_2 GPIO_NUM_5

xQueueHandle encoders_queue;

void encoder_init();

// void encoder_handle(void *arg);