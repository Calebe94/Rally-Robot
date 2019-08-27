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
#include "pid.h"

static motor_t motor1;

static uint32_t setpoint_pulses = 0;
static uint32_t pulses = 0;
static uint8_t porcentage = 35;

#define PORCENT_40  4/10
#define PORCENT_20  2/10

uint8_t falling = 0;
void pwm_handle(void *arg)
{
    pidp_t pid1 = {
        .error = 0,
        .last_sample = 0,
        .P = 0,
        .I = 0,
        .D = 0,
        .last_process = 0
    };

    pidp_t pid2 = {
        .error = 0,
        .last_sample = 0,
        .P = 0,
        .I = 0,
        .D = 0,
        .last_process = 0
    };
    pidp_t pid3 = {
        .error = 0,
        .last_sample = 0,
        .P = 0,
        .I = 0,
        .D = 0,
        .last_process = 0
    };

    while(1)
    {
        motor_set_speed(motor1, percentage_to_duty(porcentage));
        
        pulses = get_pulses_encoder_1();
        
        porcentage++;

        vTaskDelay(300/portTICK_RATE_MS);
        if(porcentage < 91 && pulses < (setpoint_pulses*PORCENT_40)+1) 
        {        
            porcentage++;
            vTaskDelay(pid_process(&pid1, setpoint_pulses*PORCENT_40, pulses) / portTICK_RATE_MS);
        }
        else if( porcentage == 90 )
        {
            vTaskDelay(pid_process(&pid2, setpoint_pulses*PORCENT_40+setpoint_pulses*PORCENT_20, pulses) / portTICK_RATE_MS);
            if ( pulses > (setpoint_pulses*PORCENT_40+setpoint_pulses*PORCENT_20))
            {
                porcentage = 89;
            }
            falling = 1;
        }
        else
        {
            porcentage--;
            vTaskDelay(pid_process(&pid3, setpoint_pulses, pulses) / portTICK_RATE_MS);
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
        xTaskCreate(&pwm_handle, "pwm_handle", 2048, NULL, 1, NULL);
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
    display_init();
    motor_timer_init();
    motor1 = motor_1_init();

    register_commands();

    serial_init();

    motor_move_foward(motor1);

    esp_console_cmd_t cmd = {
        .command = "set-pulses",
        .help = "Set pulses to init tests",
        .hint = NULL,
        .func = &set_pulses,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

int app_main(void)
{
    setup();

    display_show_header();

    while(1)
    {
        display_encoder_1_pulses(pulses);
        display_encoder_1_turns((uint32_t)pulses/16);
        if(porcentage > 99)
        {
            porcentage = 100;
        }
        display_pwm_1_porcentage(porcentage);
        vTaskDelay(50/portTICK_RATE_MS);
    }
}
