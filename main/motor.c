#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "motor.h"

void motor_1_init()
{
    gpio_pad_select_gpio(GPIO_NUM_18); // PWM

    gpio_pad_select_gpio(GPIO_NUM_15);
    gpio_pad_select_gpio(GPIO_NUM_16); // RX2

    gpio_set_direction(GPIO_NUM_18, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_15, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_16, GPIO_MODE_OUTPUT);
}

void motor1_move_foward()
{
    gpio_set_level(GPIO_NUM_18, 1);
    gpio_set_level(GPIO_NUM_16, 1);
    gpio_set_level(GPIO_NUM_15, 0);
}

void motor1_move_backward()
{
    gpio_set_level(GPIO_NUM_18, 1);
    gpio_set_level(GPIO_NUM_16, 0);
    gpio_set_level(GPIO_NUM_15, 1);
}

void motor1_stop()
{
    gpio_set_level(GPIO_NUM_18, 0);
    gpio_set_level(GPIO_NUM_16, 0);
    gpio_set_level(GPIO_NUM_15, 0);
}

void motor_2_init()
{
    gpio_pad_select_gpio(GPIO_NUM_19); // PWM

    gpio_pad_select_gpio(GPIO_NUM_17);
    gpio_pad_select_gpio(GPIO_NUM_23); // TX2

    gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_17, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT);
}
