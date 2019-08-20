
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "encoder.h"
#include "motor.h"
#include "ssd1306.h"
#include "lcd/oled_sh1106.h"

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
    sh1106_128x64_i2c_init();
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen();


    while(1)
    {
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}
