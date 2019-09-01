#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <time.h>

#include "freertos/semphr.h"

void display_init(void);

void display_show_header();

void display_encoder_1_pulses(uint32_t pulses);

void display_encoder_1_turns(uint32_t turns);

void display_pwm_1_porcentage(uint8_t porcentage);

void display_sensor_status_table();

void display_sensor_status(bool status1, bool status2, bool status3);