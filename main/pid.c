#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "config.h"
#include "pid.h"

// static uint32_t error = 0;
// static uint32_t last_sample = 0;
// static uint32_t P = 0;
// static uint32_t I = 0;
// static uint32_t D = 0;
// static long last_process = 0;

static long millis()
{
    return (long)xTaskGetTickCount()*portTICK_RATE_MS;
}

double pid_process(pidp_t *pid, uint32_t setpoint, uint32_t sample)
{
    pid->error = setpoint - sample;
    
    float delta_time = ((millis()) - pid->last_process)/1000.0;
    pid->last_process = millis();

    pid->P = pid->error * PID_KP;

    pid->I = pid->I + (pid->error * PID_KI) * delta_time;

    pid->D = (pid->last_sample - sample) * PID_KD / delta_time;
    pid->last_sample = sample;

    return (double) pid->P + pid->I + pid->D;
}

// double pid_process(uint32_t setpoint, uint32_t sample)
// {
//     error = setpoint - sample;
    
//     float delta_time = ((millis()) - last_process)/1000.0;
//     last_process = millis();

//     P = error * PID_KP;

//     I = I + (error * PID_KI) * delta_time;

//     D = (last_sample - sample) * PID_KD /delta_time;
//     last_sample = sample;

//     return (double) P + I + D;
// }
