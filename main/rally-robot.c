
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "encoder.h"
#include "motor.h"

static int pulses = 0;

static void encoders_handle(void* arg)
{
    uint32_t io_num;
    for(;;) 
    {
        if(xQueueReceive(encoders_queue, &io_num, portMAX_DELAY)) 
        {
            if(io_num == ENCODER_1)
            {
                pulses++;
                printf("pulses %d - turns %d \n", pulses, (int)pulses/16);
            }
        }
    }
}

void setup()
{
    encoder_init();
    xTaskCreate(encoders_handle, "encoders_handle", 2048, NULL, 10, NULL);
}

int app_main(void)
{
    setup();

    while(1)
    {
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}
