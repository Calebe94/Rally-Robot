#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "ssd1306.h"
#include "lcd/oled_sh1106.h"

#include "display.h"
#include "encoder.h"


static char tag[] = "display";

static int pulses = 0;

char pulses_string[20]; 
char turns_string[20];
char string_porcentage[20];

static void task_display_print_info(void *arg)
{
    uint32_t io_num;
    
    ssd1306_printFixed(0,  8, "Encoder 1", STYLE_BOLD);
    
    for(;;) 
    {
        if(xQueueReceive(encoders_queue, &io_num, portMAX_DELAY)) 
        {
            if(io_num == ENCODER_1)
            {
                pulses++;
                // printf("pulses %d - turns %d \n", pulses, (int)pulses/16);
                sprintf(pulses_string, "pulses: %d", pulses);
                sprintf(turns_string, "turns: %d", (int)pulses/16);
                ssd1306_printFixed(0, 24, pulses_string, STYLE_NORMAL);
                ssd1306_printFixed(0, 38, turns_string, STYLE_NORMAL);
                ssd1306_printFixed(0, 52, string_porcentage, STYLE_NORMAL);
            }
        }
    }    
}

void display_pwm(uint8_t porcentage)
{
    sprintf(string_porcentage, "PWM: %d", porcentage);
}

void display_setup(void)
{
    sh1106_128x64_i2c_init();
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen();
    xTaskCreate(task_display_print_info, "task_display_print_info", 2048, NULL, 10, NULL);
}