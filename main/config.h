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
#define MOTOR_1_A        GPIO_NUM_16
#define MOTOR_1_B        GPIO_NUM_15

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

/*******************************
**********LINE SENSOR***********
*******************************/

#define LINE_SENSOR_1   GPIO_NUM_34
#define LINE_SENSOR_2   GPIO_NUM_35
#define LINE_SENSOR_3   GPIO_NUM_32

/*******************************
*******ULTRASONIC SENSOR********
*******************************/

#define MAX_DISTANCE_CM 100 // 5m max

#define TRIGGER_GPIO_LEFT       GPIO_NUM_26
#define ECHO_GPIO_LEFT          GPIO_NUM_25

#define TRIGGER_GPIO_RIGHT      GPIO_NUM_27
#define ECHO_GPIO_RIGHT         GPIO_NUM_2

#define TRIGGER_GPIO_FRONT      GPIO_NUM_21
#define ECHO_GPIO_FRONT         GPIO_NUM_22