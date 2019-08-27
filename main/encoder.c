#include <stdint.h>
#include "driver/gpio.h"

#include "config.h"
#include "encoder.h"

static uint32_t pulses_encoder_1 = 0;
static uint32_t pulses_encoder_2 = 0;

static void IRAM_ATTR encoder_1_handle(void *arg)
{
    pulses_encoder_1++;
}

static void IRAM_ATTR encoder_2_handle(void *arg)
{
    pulses_encoder_2++;
}

void encoders_init()
{
    gpio_config_t io_conf;

    //interrupt of rising edge
    // io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = ((1ULL<<ENCODER_1) | (1ULL<<ENCODER_2));
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 0;

    gpio_config(&io_conf);

    gpio_install_isr_service(0);
}

void encoders_start()
{
    gpio_isr_handler_add(ENCODER_1, encoder_1_handle, (void*) ENCODER_1);
    gpio_isr_handler_add(ENCODER_2, encoder_2_handle, (void*) ENCODER_2);
}

uint32_t get_pulses_encoder_1()
{
    return pulses_encoder_1;
}

uint32_t get_pulses_encoder_2()
{
    return pulses_encoder_2;
}