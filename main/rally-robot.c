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

void handle_pulses(void *setpoint)
{
    // int pulses = (int)*setpoint;
    // pulses=setpoint_pulses;
    
    int pwmMin=35;
    int pwmMax=60;
    int pulseR = 0;
    int pwm = 0;
    int pwm2 = 0;
    int rangeAcc=(setpoint_pulses/100)*20;
    int rangeDeacc=(setpoint_pulses/100)*30;
    int rangeMax=setpoint_pulses-rangeAcc-rangeDeacc;
    // printf("Task Created!\n");
    int pulR = 0;

    for(pwm = pwmMin; pwm <= pwmMax; pwm++)
    {
        pulR = get_pulses_encoder_1();
        motor_set_speed(motor1, percentage_to_duty(pwm));
        printf("Rising pulR: %d - rangeAcc: %d - PWM Porcentage: %d - Pulses: %d\n",pulR, rangeAcc, pwm, pulR);
        //set(pwm)
        vTaskDelay(10/portTICK_RATE_MS);
        // readpulse

        if (pulR <= rangeAcc)
        {
            break;
        }
    }

    while(pulR < rangeMax)
    {
        pulR = get_pulses_encoder_1();
    }

    for(pwm2 = pwmMax; pwm2 <= pwmMin; pwm2--)
    {
        // sleep 1
        //set(pwm2)
        motor_set_speed(motor1, percentage_to_duty(pwm2));
        pulR = get_pulses_encoder_1();
        printf("PWM Porcentage: %d - Pulses: %d\n", pwm2, pulR);
        // vTaskDelay(100/portTICK_RATE_MS);
        int pulseR = 0;
        if (pulR <= rangeDeacc)
        {
            break;
        }
    }
}
void pwm_handle(void *arg)
{
    int pwmMin=35;
    int pwmMax=90;
    int pulseR = 0;
    int pwm = 0;
    int pwm2 = 0;
    int rangeAcc=(setpoint_pulses/100)*23;
    int rangeDeacc=setpoint_pulses-300;
    int rangeMax=setpoint_pulses-rangeAcc-rangeDeacc;

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
        // handle_pulses(NULL);
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
