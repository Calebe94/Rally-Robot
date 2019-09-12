#include <stdio.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "config.h"
#include "encoder.h"
#include "motor.h"
#include "display.h"
#include "serial.h"
#include "commands.h"
#include "line_sensor.h"
#include "pid.h"

static motor_t motor1;
static motor_t motor2;

static uint8_t movement = 0;

static uint32_t setpoint_pulses = 0;
static uint32_t pulses = 0;
static uint8_t porcentage = 35;

void pwm_handle(void *arg)
{
    const int pwmMax=90;
    int rangeAcc=(setpoint_pulses/100)*23;
    int rangeDeacc=setpoint_pulses-300;

    while(1)
    {
        motor_set_speed(motor1, percentage_to_duty(porcentage));
        pulses = get_pulses_encoder_1();

        printf("rangeAcc: %d - rangeDeacc: %d - PWM Porcentage: %d - Pulses: %d\n", rangeAcc, rangeDeacc, porcentage, pulses);

        if(porcentage < pwmMax && pulses < rangeAcc)
        {
            porcentage++;
            vTaskDelay(100/portTICK_RATE_MS);
            if(porcentage == 100)
            {
                porcentage = 100;
            }
        }
        else if(pulses > rangeDeacc )
        {
            porcentage--;
            if (pulses < setpoint_pulses && porcentage < 30)
            {
                porcentage = 30;
            }
            else if(pulses > setpoint_pulses && porcentage > 0)
            {
                porcentage = 0;
            }
            vTaskDelay(100/portTICK_RATE_MS);
        }
    }
}

static int set_pulses(int argc, char **argv)
{
    if (argc > 1)
    {
        setpoint_pulses = atoi(argv[1]);
        printf("\nPulses setted to %d\n", setpoint_pulses);
        encoders_start();
        xTaskCreate(&pwm_handle, "pwm_handle", 4096, NULL, 1, NULL);
    }
    else
    {
        printf("\nInvalid...\n");
    }
    return 0;
}

static void setup()
{
    encoders_init();
    encoders_start();
    display_init();
    motor_timer_init();
    motor1 = motor_1_init();
    motor2 = motor_2_init();
    line_sensor_init();
    line_sensor_start();

    // register_commands();

    // serial_init();

    motor_move_foward(motor1);

    esp_console_cmd_t cmd = {
        .command = "set-pulses",
        .help = "Set pulses to init tests",
        .hint = NULL,
        .func = &set_pulses,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void demo_actuators(void *args)
{
    motor_set_speed(motor1, percentage_to_duty(100));
    motor_set_speed(motor2, percentage_to_duty(100));

    while (1)
    {
        motor_move_foward(motor1);
        motor_move_foward(motor2);
        movement = 1;
        vTaskDelay(5000/portTICK_RATE_MS);

        motor_move_backward(motor1);
        motor_move_backward(motor2);
        movement = 2;

        vTaskDelay(5000/portTICK_RATE_MS);

        motor_move_foward(motor1);
        motor_stop(motor2);
        movement = 3;

        vTaskDelay(5000/portTICK_RATE_MS);

        motor_move_foward(motor2);
        motor_stop(motor1);
        movement = 4;

        vTaskDelay(5000/portTICK_RATE_MS);

        motor_stop(motor1);
        motor_stop(motor2);
        movement = 0;
    }
}

static void demo_sensors(void *args)
{
    display_show_header();
    display_sensor_status_table();
    // display_movement("Foward");    
    pidp_t pid = {
        .error = 0,
        .last_sample = 0,
        .P = 0,
        .I = 0,
        .D = 0,
        .last_process = 0
    };
    
    while(1)
    {
        display_encoder_1_pulses(get_pulses_encoder_1());
        display_encoder_2_pulses(get_pulses_encoder_2());
        display_encoder_1_turns((uint32_t)get_pulses_encoder_1()/16);
        display_encoder_2_turns((uint32_t)get_pulses_encoder_2()/16);
        switch(movement)
        {
            case 0: 
                display_movement("Stop");
                break;
            case 1: 
                display_movement("Foward");
                break;
            case 2: 
                display_movement("Backward");
                break;
            case 3: 
                display_movement("Left");
                break;
            case 4: 
                display_movement("Right");
                break;
        }

        if(porcentage > 99)
        {
            porcentage = 100;
        }
        printf("PID: %d\n", pid_process(&pid, get_pulses_encoder_2(), get_pulses_encoder_1()));

        display_pwm_1_porcentage(porcentage);
        display_sensor_status(get_line_sensor_1_status(), get_line_sensor_2_status(), get_line_sensor_3_status());
        vTaskDelay(50/portTICK_RATE_MS);
    }
}

/***************************************************************************************************
 * Motors
***************************************************************************************************/

void move_foward()
{
    motor_move_foward(motor1);
    motor_move_foward(motor2);
}

void move_backward()
{
    motor_move_backward(motor1);
    motor_move_backward(motor2);
}

void move_right()
{
    motor_move_foward(motor2);
    motor_stop(motor1);
}

void move_left()
{
    motor_move_foward(motor1);
    motor_stop(motor2);
}

void stop()
{
    motor_stop(motor1);
    motor_stop(motor2);
}

void task_rally_robot(void *arg)
{
    // Check: https://www.hackster.io/16336/line-follower-robot-using-arduino-5f5698
    while(1)
    {
        if(get_line_sensor_1_status() && get_line_sensor_2_status() && get_line_sensor_3_status())
        {
            stop();
        }
        else
        {
            move_foward();
        }

        if(get_line_sensor_3_status())
        {
            move_left();
        }

        if(get_line_sensor_1_status())
        {
            move_right();
        }

        if(get_line_sensor_1_status())
        {
            move_right();
        }
        

    }
}

int app_main(void)
{
    setup();

    xTaskCreate(&demo_actuators, "demo_actuators", 4096, NULL, 1, NULL);
    xTaskCreate(&demo_sensors, "demo_sensors", 4096, NULL, 1, NULL);

    while(1)
    {
        vTaskDelay(50/portTICK_RATE_MS);
    }
}
