#include <stdio.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "encoder.h"

static void IRAM_ATTR encoder_handle(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(encoders_queue, &gpio_num, NULL);
}

void encoder_init()
{
    gpio_config_t io_conf;

    //interrupt of rising edge
    // io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = ((1ULL<<ENCODER_1) | (1ULL<<ENCODER_2));
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 0;

    gpio_config(&io_conf);

    gpio_install_isr_service(0);

    gpio_isr_handler_add(ENCODER_1, encoder_handle, (void*) ENCODER_1);
    gpio_isr_handler_add(ENCODER_2, encoder_handle, (void*) ENCODER_2);

    encoders_queue = xQueueCreate(10, sizeof(uint32_t));
}
