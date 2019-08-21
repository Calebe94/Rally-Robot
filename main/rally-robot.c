
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "encoder.h"
#include "motor.h"
#include "display.h"

void setup()
{
    encoder_init();
    display_setup();
}

int app_main(void)
{
    setup();

    while(1)
    {
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}
