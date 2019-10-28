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
#include <ultrasonic.h>

//==================WARNING==========================
// GPIO 13 ou GPIO 12 estão com problemas. Verificar!
//==================WARNING==========================

// tempo 39
// distance 6 metros
static motor_t esquerda;
static motor_t direita;

static uint8_t movement = 0;

static uint32_t setpoint_pulses = 0;
static uint32_t pulses = 0;
static uint8_t porcentage = 35;

#define MAX_DISTANCE_CM 100 // 5m max

#define TRIGGER_GPIO_LEFT       26
#define ECHO_GPIO_LEFT          25

#define TRIGGER_GPIO_RIGHT      27
#define ECHO_GPIO_RIGHT         2

void ultrasonic_test(void *pvParamters)
{
    ultrasonic_sensor_t ultrasonic_left = {
        .trigger_pin = TRIGGER_GPIO_LEFT,
        .echo_pin = ECHO_GPIO_LEFT
    };

    ultrasonic_sensor_t ultrasonic_right = {
        .trigger_pin = TRIGGER_GPIO_RIGHT,
        .echo_pin = ECHO_GPIO_RIGHT
    };

    ultrasonic_init(&ultrasonic_left);
    ultrasonic_init(&ultrasonic_right);
    
    // gpio_set_level(ultrasonic_right.trigger_pin, 1);

    motor_set_speed(esquerda, percentage_to_duty(40));
    motor_set_speed(direita, percentage_to_duty(40));
    motor_move_foward(esquerda);
    motor_move_foward(direita);

    uint32_t lines = 0;

    while (true)
    {
        uint32_t distance_left;
        uint32_t distance_right;

        ultrasonic_measure_cm(&ultrasonic_left, MAX_DISTANCE_CM, &distance_left);
        printf("Distance LEFT <<<: %d cm\n", distance_left);

        ultrasonic_measure_cm(&ultrasonic_right, MAX_DISTANCE_CM, &distance_right);
        printf("Distance RIGHT >>>: %d cm\n", distance_right);

        if (get_line_sensor_1_status() || get_line_sensor_2_status() || get_line_sensor_3_status())
        {
            lines++;
        }

        if (distance_right > distance_left)
        {
            motor_set_speed(esquerda, percentage_to_duty(40));
            motor_set_speed(direita, percentage_to_duty(35));
        }
        else if (distance_right < distance_left)
        {
            motor_set_speed(esquerda, percentage_to_duty(35));
            motor_set_speed(direita, percentage_to_duty(40));
        }
        else if(distance_right == distance_left)
        {
            motor_set_speed(esquerda, percentage_to_duty(40));
            motor_set_speed(direita, percentage_to_duty(40));
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void pwm_handle(void *arg)
{
    const int pwmMax=90;
    int rangeAcc=(setpoint_pulses/100)*23;
    int rangeDeacc=setpoint_pulses-300;

    while(1)
    {
        motor_set_speed(esquerda, percentage_to_duty(porcentage));
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
    esquerda = motor_1_init();
    direita = motor_2_init();
    line_sensor_init();
    line_sensor_start();

    // register_commands();

    // serial_init();

    motor_move_foward(esquerda);

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
    motor_set_speed(esquerda, percentage_to_duty(100));
    motor_set_speed(direita, percentage_to_duty(100));

    while (1)
    {
        motor_move_foward(esquerda);
        motor_move_foward(direita);
        movement = 1;
        vTaskDelay(5000/portTICK_RATE_MS);

        motor_move_backward(esquerda);
        motor_move_backward(direita);
        movement = 2;

        vTaskDelay(5000/portTICK_RATE_MS);

        motor_move_foward(esquerda);
        motor_stop(direita);
        movement = 3;

        vTaskDelay(5000/portTICK_RATE_MS);

        motor_move_foward(direita);
        motor_stop(esquerda);
        movement = 4;

        vTaskDelay(5000/portTICK_RATE_MS);

        motor_stop(esquerda);
        motor_stop(direita);
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

    motor_set_speed(esquerda, percentage_to_duty(50));
    motor_set_speed(direita, percentage_to_duty(50));
    motor_move_foward(esquerda);
    motor_move_foward(direita);

    while(1)
    {
        // display_encoder_1_pulses(get_pulses_encoder_1());
        // display_encoder_2_pulses(get_pulses_encoder_2());
        // display_encoder_1_turns((uint32_t)get_pulses_encoder_1()/16);
        // display_encoder_2_turns((uint32_t)get_pulses_encoder_2()/16);
        // switch(movement)
        // {
        //     case 0:
        //         display_movement("Stop");
        //         break;
        //     case 1:
        //         display_movement("Foward");
        //         break;
        //     case 2:
        //         display_movement("Backward");
        //         break;
        //     case 3:
        //         display_movement("Left");
        //         break;
        //     case 4:
        //         display_movement("Right");
        //         break;
        // }

        // if(porcentage > 99)
        // {
        //     porcentage = 100;
        // }
        // printf("PID: %d\n", pid_process(&pid, get_pulses_encoder_2(), get_pulses_encoder_1()));
        if (get_line_sensor_1_status() || get_line_sensor_2_status() || get_line_sensor_3_status())
        {
            motor_stop(esquerda);
            motor_stop(direita);

            motor_set_speed(esquerda, percentage_to_duty(0));
            motor_set_speed(direita, percentage_to_duty(0));

        }
        printf("Sensor 1: %d \t Sensor 2: %d \t Sensor 3: %d\n", get_line_sensor_1_status(), get_line_sensor_2_status(), get_line_sensor_3_status());
        // display_pwm_1_porcentage(porcentage);
        // display_sensor_status(get_line_sensor_1_status(), get_line_sensor_2_status(), get_line_sensor_3_status());
        vTaskDelay(50/portTICK_RATE_MS);
    }
}

/***************************************************************************************************
 * Motors
***************************************************************************************************/

void move_foward()
{
    motor_move_foward(esquerda);
    motor_move_foward(direita);
    movement = 1;
}

void move_backward()
{
    motor_move_backward(esquerda);
    motor_move_backward(direita);
    movement = 2;
}

void move_right()
{
    motor_move_foward(direita);
    // motor_stop(esquerda);
    movement = 4;
}

void move_left()
{
    motor_move_foward(esquerda);
    // motor_stop(direita);
    movement = 3;
}

void stop()
{
    motor_stop(esquerda);
    motor_stop(direita);
    movement = 0;
}

void task_rally_robot(void *arg)
{
    motor_set_speed(esquerda, percentage_to_duty(60));
    motor_set_speed(direita, percentage_to_duty(60));

    // Check: https://www.hackster.io/16336/line-follower-robot-using-arduino-5f5698
    while(1)
    {
        if(get_line_sensor_1_status() == 0 && get_line_sensor_2_status() == 0 && get_line_sensor_3_status() == 0)
        {
            motor_set_speed(esquerda, percentage_to_duty(0));
            motor_set_speed(direita, percentage_to_duty(0));
            stop();
        }
        else
        {
            motor_set_speed(esquerda, percentage_to_duty(60));
            motor_set_speed(direita, percentage_to_duty(60));
            move_foward();
        }

        if(get_line_sensor_3_status())
        {
            motor_set_speed(direita, percentage_to_duty(0));
            move_left();
        }

        if(get_line_sensor_1_status())
        {
            motor_set_speed(esquerda, percentage_to_duty(0));
            move_right();
        }

        if(get_line_sensor_2_status())
        {
            move_right();
        }
    }
}

void task_rally_robot_artur(void *arg)
{
    int valorSensor1=1;
    int valorSensor2=1;
    int valorSensor3=1;

    int memoriaSensor1=0;
    int memoriaSensor2=0;
    int memoriaSensor3=0;

    motor_set_speed(esquerda, percentage_to_duty(40));
    motor_set_speed(direita, percentage_to_duty(40));


    while(1)
    {
        valorSensor1 = get_line_sensor_1_status();
        valorSensor2 = get_line_sensor_2_status();
        valorSensor3 = get_line_sensor_3_status();
        move_foward(esquerda);
        move_foward(direita);
        // Todos acionados, saiu da linha, parar carrinho
        if( valorSensor1 == 0 &&
        valorSensor2 == 0 &&
        valorSensor3 == 0)
        {
            valorSensor1 = memoriaSensor1;
            valorSensor2 = memoriaSensor2;
            valorSensor3 = memoriaSensor3;
            motor_set_speed(esquerda, percentage_to_duty(35));
            motor_set_speed(direita, percentage_to_duty(35));
            move_foward();
            // vTaskDelay(10/portTICK_RATE_MS);
        }

        // virar pra esquerda, 011, virar pouco pra esquerda
        if( valorSensor1 == 0 &&
        valorSensor2 == 1 &&
        valorSensor3 == 1)
        {
            motor_set_speed(esquerda, percentage_to_duty(35));
            motor_set_speed(direita, percentage_to_duty(15));
            // vTaskDelay(10/portTICK_RATE_MS);
            // move_left();
        }
        // virar pra esquerda, 001, virar muito pra esquerda
        else if(valorSensor1 == 0 &&
            valorSensor2 == 0 &&
            valorSensor3 == 1)
        {
            motor_set_speed(esquerda, percentage_to_duty(37));
            motor_set_speed(direita, percentage_to_duty(8));
            // vTaskDelay(10/portTICK_RATE_MS);
            // move_left();
        }
        // virar pra direita, 100, virar muito pra direita
        else if(valorSensor1 == 1 &&
            valorSensor2 == 0 &&
            valorSensor3 == 0)
        {
            motor_set_speed(esquerda, percentage_to_duty(8));
            motor_set_speed(direita, percentage_to_duty(37));
            // vTaskDelay(10/portTICK_RATE_MS);
            // move_right();
        }
        // virar pra direita, 110, virar pouco pra direita
        else if(valorSensor1 == 1 &&
            valorSensor2 == 1 &&
            valorSensor3 == 0)
        {
            motor_set_speed(esquerda, percentage_to_duty(15));
            motor_set_speed(direita, percentage_to_duty(35));
            // vTaskDelay(10/portTICK_RATE_MS);
            // move_right();
        }
        // parar, 000, tudo preto bizarro
        else if(valorSensor1 == 1 &&
            valorSensor2 == 1 &&
            valorSensor3 == 1)
        {
            // motor_set_speed(esquerda, percentage_to_duty(0));
            // motor_set_speed(direita, percentage_to_duty(0));
            // stop();

        }
        else if(valorSensor1 == 0 &&
            valorSensor2 == 1 &&
            valorSensor3 == 0)
        {
            motor_set_speed(esquerda, percentage_to_duty(35));
            motor_set_speed(direita, percentage_to_duty(35));
            // vTaskDelay(10/portTICK_RATE_MS);
            // move_foward();
        }

        memoriaSensor1 = valorSensor1;
        memoriaSensor2 = valorSensor2;
        memoriaSensor3 = valorSensor3;
    }
}

int app_main(void)
{
    setup();
    xTaskCreate(ultrasonic_test, "ultrasonic_test", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);

    // xTaskCreate(&demo_actuators, "demo_actuators", 4096, NULL, 1, NULL);
    // xTaskCreate(&demo_sensors, "demo_sensors", 4096, NULL, 1, NULL);
    // xTaskCreate(&task_rally_robot, "task_rally_robot", 4096, NULL, 1, NULL);
    // xTaskCreate(&task_rally_robot_artur, "task_rally_robot_artur", 4096, NULL, 1, NULL);
    while(1)
    {
        vTaskDelay(50/portTICK_RATE_MS);
    }
}
