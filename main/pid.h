#include <stdint.h>

struct pid 
{
    uint32_t error;
    uint32_t last_sample;
    uint32_t P;
    uint32_t I;
    uint32_t D;
    long last_process;
};

typedef struct pid pidp_t;

double pid_process(pidp_t *, uint32_t, uint32_t);

// double pid_process(uint32_t, uint32_t);
