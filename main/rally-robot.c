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
    display_init();
    motor_timer_init();
    motor1 = motor_1_init();
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

int app_main(void)
{
    setup();

    display_show_header();
    display_sensor_status_table();

    while(1)
    {
        display_encoder_1_pulses(pulses);
        display_encoder_1_turns((uint32_t)pulses/16);
        if(porcentage > 99)
        {
            porcentage = 100;
        }
        display_pwm_1_porcentage(porcentage);
        display_sensor_status(get_line_sensor_1_status(), get_line_sensor_2_status(), get_line_sensor_3_status());
        printf("| LS1: %d | LS2: %d | LS3: %d |\n", get_line_sensor_1_status(), get_line_sensor_2_status(), get_line_sensor_3_status());
        vTaskDelay(50/portTICK_RATE_MS);
    }
}
