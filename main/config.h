#include "driver/gpio.h"
#include "driver/ledc.h"

/*******************************
*************MOTORS*************
*******************************/

/*
    ledc timer attributes
*/
#define MOTOR_TIMER             LEDC_TIMER_0
#define MOTOR_MODE              LEDC_HIGH_SPEED_MODE
#define MOTOR_PWM_FREQ_HZ       1000
#define MOTOR_DUTY_RES          LEDC_TIMER_13_BIT

/*
    Motor 1 specific attributes
*/
#define MOTOR_1_GPIO     GPIO_NUM_18
#define MOTOR_1_CHANNEL  LEDC_CHANNEL_0
#define MOTOR_1_A        GPIO_NUM_15
#define MOTOR_1_B        GPIO_NUM_16

/*
    Motor 2 specific attributes
*/
#define MOTOR_2_GPIO     GPIO_NUM_19
#define MOTOR_2_CHANNEL  LEDC_CHANNEL_1
#define MOTOR_2_A        GPIO_NUM_17
#define MOTOR_2_B        GPIO_NUM_23

/*******************************
************ENCODERS************
*******************************/

#define ENCODER_1 GPIO_NUM_4
#define ENCODER_2 GPIO_NUM_5

/*******************************
*************DISPLAY************
*******************************/

#define TIMEOUT_SCREEN  10000

/*******************************
***************PID**************
*******************************/

#define PID_KP  1   // Defaul 1
#define PID_KI  0   // Defaul 0
#define PID_KD  0   // Defaul 0
