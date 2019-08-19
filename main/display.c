#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <time.h>

#include "display.h"
#include "freertos/semphr.h"

static char tag[] = "display";

u8g2_t display_setup(void){
  // initialize the u8g2 hal
  u8g2_t u8g2;
  u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
  u8g2_esp32_hal.sda = PIN_SDA;
  u8g2_esp32_hal.scl = PIN_SCL;
  u8g2_esp32_hal_init(u8g2_esp32_hal);

  // initialize the u8g2 library
  u8g2_Setup_ssd1306_128x64_noname_f(
    &u8g2,
    U8G2_R0,
    u8g2_esp32_msg_i2c_cb,
    u8g2_esp32_msg_i2c_and_delay_cb);

  u8x8_SetI2CAddress(&u8g2.u8x8, 0x78);   // set the display address
  u8g2_InitDisplay(&u8g2);                // initialize the display
  u8g2_SetPowerSave(&u8g2, 0);            // wake up the display
  return u8g2;
}