#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ssd1306.h"
#include "lcd/oled_sh1106.h"

#include "display.h"
#include "encoder.h"

static char tag[] = "display";

void display_init(void)
{
    sh1106_128x64_i2c_init();
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen();
}

void display_show_header()
{
    ssd1306_printFixed(0,  8, "Encoder 1", STYLE_BOLD);
}

void display_encoder_1_pulses(uint32_t pulses)
{
    char pulses_string[20];
    sprintf(pulses_string, "pulses: %d", pulses);
    ssd1306_printFixed(0, 24, pulses_string, STYLE_NORMAL);
}

void display_encoder_1_turns(uint32_t turns)
{
    char turns_string[20];
    sprintf(turns_string, "turns: %d", turns);
    ssd1306_printFixed(0, 38, turns_string, STYLE_NORMAL);
}

void display_pwm_1_porcentage(uint8_t porcentage)
{
    char string_porcentage[20];
    sprintf(string_porcentage, "PWM 1: %d", porcentage);
    ssd1306_printFixed(0, 52, string_porcentage, STYLE_NORMAL);
}