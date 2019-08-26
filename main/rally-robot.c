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

static motor_t motor1;

static int pulses = 0;

static int set_pulses(int argc, char **argv)
{
    if (argc > 1)
    {
        pulses = atoi(argv[1]);
        printf("\nPulses setted to %d\n", pulses);
    }
    else
    {
        printf("\nInvalid...\n");
    }
    return 0;
}

// MAX 8192
// MID 4096
// MIN 0
/*
    100% = 8192
    x    = y
    x*8192 = 100*y
    
    y=x*8192/100
*/

void setup()
{
    encoder_init();
    display_setup();

    motor_timer_init();

    register_commands();

    serial_init();

    motor1 = motor_1_init();

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

    // uint8_t porcentage = 0;

    while(1)
    {
        // porcentage+=10;
        // motor_set_speed(motor1, percentage_to_duty(porcentage));
        // printf("Porcentage PWM: %d\n", porcentage);
        // display_pwm(porcentage);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
