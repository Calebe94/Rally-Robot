#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <time.h>

#include "freertos/semphr.h"

void display_setup(void);

void display_pwm(uint8_t porcentage);