#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <time.h>

#include "u8g2_esp32_hal.h"
#include "freertos/semphr.h"

#define PIN_SDA 21
#define PIN_SCL 22

u8g2_t display;

u8g2_t display_setup(void);
