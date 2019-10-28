#include <stdbool.h>
#include "driver/gpio.h"

#include "config.h"
#include "line_sensor.h"

static bool sensor_1_status = false;
static bool sensor_2_status = false;
static bool sensor_3_status = false;

static void IRAM_ATTR line_sensor_1_handle(void *arg)
{
    sensor_1_status = (bool)gpio_get_level(LINE_SENSOR_1);
}

static void IRAM_ATTR line_sensor_2_handle(void *arg)
{
    sensor_2_status = (bool)gpio_get_level(LINE_SENSOR_2);
}

static void IRAM_ATTR line_sensor_3_handle(void *arg)
{
    sensor_3_status = (bool)gpio_get_level(LINE_SENSOR_3);
}

void line_sensor_init()
{
    gpio_config_t io_conf;

    //interrupt of rising edge
    // io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = ((1ULL<<LINE_SENSOR_1) | (1ULL<<LINE_SENSOR_2) | (1ULL<<LINE_SENSOR_3));
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 0;

    gpio_config(&io_conf);

    // gpio_install_isr_service(0);
}

void line_sensor_start()
{
    gpio_isr_handler_add(LINE_SENSOR_1, line_sensor_1_handle, NULL);
    gpio_isr_handler_add(LINE_SENSOR_2, line_sensor_2_handle, NULL);
    gpio_isr_handler_add(LINE_SENSOR_3, line_sensor_3_handle, NULL);
}

bool get_line_sensor_1_status()
{
    return sensor_1_status;
}

bool get_line_sensor_2_status()
{
    return sensor_2_status;
}

bool get_line_sensor_3_status()
{
    return sensor_3_status;
}
