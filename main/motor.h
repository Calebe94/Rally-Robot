#include <stdint.h>
#include "driver/ledc.h"

struct motor 
{
    uint8_t gpio_a;
    uint8_t gpio_b;
    ledc_channel_config_t pwm;
};

typedef struct motor motor_t;

/*
    Convert percentage to duty cycle
*/
uint16_t percentage_to_duty(uint8_t percentage);

/*
    Init ledc timer
*/
void motor_timer_init();

/*
    Initialize motor1
*/
motor_t motor_1_init();

/*
    Initialize motor2
*/
motor_t motor_2_init();

/*
    Set motor Speed.
*/
void motor_set_speed(motor_t, uint16_t);

/*
    Makes a given motor fowards
*/
void motor_move_foward(motor_t);

/*
    Makes a given motor backwards
*/
void motor_move_backward(motor_t);

/*
    Makes a given motor stop
*/
void motor_stop(motor_t);