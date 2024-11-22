#include "detect.h"
#include "main.h"

// Output led to debug
static DigitalOut led(LED1);

// Power variables
uint64_t historical_power = 0;
static uint32_t POWER_THRESHOLD_1 = 20000000; // (max 2^32-1 = 4.294.967.295)
static uint64_t POWER_WINDOW = 2; // in reallity, it is times 16

// buffer_size < 2^32
uint64_t power_calculation(int16_t *buffer, uint32_t buffer_size) {
    uint64_t energy = 0;
    for (int ix = 0; ix < buffer_size; ix++) {
        energy += buffer[ix]*buffer[ix]; // buffer[ix]*buffer[ix] -> 32 bits
    }
    return energy/buffer_size; // maximum 32 bits
}

void detect_voice_1(int16_t *buffer, uint32_t buffer_size ) {   
    uint64_t this_power = power_calculation(buffer,  buffer_size);
    uint64_t power = ((POWER_WINDOW-1) * historical_power + this_power)/POWER_WINDOW;
    if (power >= POWER_THRESHOLD_1) {
        led = 1;
        if (!SAVING_AUDIO){
            SAVING_AUDIO = true;
        }
    } else {
        led = 0;
    }
    historical_power = power;
}