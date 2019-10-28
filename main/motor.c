#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "config.h"
#include "motor.h"

uint16_t percentage_to_duty(uint8_t percentage)
{
    if (percentage > 99)
    {
        percentage = 100;
    }
    return (uint16_t) percentage*pow(2, MOTOR_DUTY_RES)/100;
}

void motor_timer_init()
{
    /*
     * Prepare and set configuration of timers
     * that will be used by LED Controller
     */
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = MOTOR_DUTY_RES,   // resolution of PWM duty
        .freq_hz = MOTOR_PWM_FREQ_HZ,                        // frequency of PWM signal
        .speed_mode = MOTOR_MODE,               // timer mode
        .timer_num = MOTOR_TIMER,                // timer index
    };
    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);
}

motor_t motor_1_init()
{
    ledc_channel_config_t ledc_channel = {
        .channel    = MOTOR_1_CHANNEL,
        .duty       = 0,
        .gpio_num   = MOTOR_1_GPIO,
        .speed_mode = MOTOR_MODE,
        .timer_sel  = MOTOR_TIMER
    };

    motor_t aux = {
        .pwm = ledc_channel,
        .gpio_a = MOTOR_1_A,
        .gpio_b = MOTOR_1_B,
    };

    ledc_channel_config(&ledc_channel);

    gpio_pad_select_gpio(aux.gpio_a);
    gpio_pad_select_gpio(aux.gpio_b);

    gpio_set_direction(aux.gpio_a, GPIO_MODE_OUTPUT);
    gpio_set_direction(aux.gpio_b, GPIO_MODE_OUTPUT);

    return aux;
}

motor_t motor_2_init()
{
    ledc_channel_config_t ledc_channel = {
        .channel    = MOTOR_2_CHANNEL,
        .duty       = 0,
        .gpio_num   = MOTOR_2_GPIO,
        .speed_mode = MOTOR_MODE,
        .timer_sel  = MOTOR_TIMER
    };

    motor_t aux = {
        .pwm = ledc_channel,
        .gpio_a = MOTOR_2_A,
        .gpio_b = MOTOR_2_B,
    };
    ledc_channel_config(&aux.pwm);

    gpio_pad_select_gpio(aux.gpio_a);
    gpio_pad_select_gpio(aux.gpio_b);

    gpio_set_direction(aux.gpio_a, GPIO_MODE_OUTPUT);
    gpio_set_direction(aux.gpio_b, GPIO_MODE_OUTPUT);

    return aux;
}

void motor_set_speed(motor_t motor, uint16_t speed)
{
    ledc_set_duty(motor.pwm.speed_mode, motor.pwm.channel, speed);
    ledc_update_duty(motor.pwm.speed_mode, motor.pwm.channel);
}

void motor_move_foward(motor_t motor)
{
    gpio_set_level(motor.gpio_a, 1);
    gpio_set_level(motor.gpio_b, 0);
}

void motor_move_backward(motor_t motor)
{
    gpio_set_level(motor.gpio_a, 0);
    gpio_set_level(motor.gpio_b, 1);
}

void motor_stop(motor_t motor)
{
    gpio_set_level(motor.gpio_a, 0);
    gpio_set_level(motor.gpio_b, 0);
}
