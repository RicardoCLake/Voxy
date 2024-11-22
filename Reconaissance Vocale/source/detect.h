#include "mbed.h"
#include <cstdint>

#ifndef _DETECT_HPP
#define _DETECT_HPP

extern uint64_t historical_power;

// Functions
uint64_t power_calculation(int16_t *buffer, uint32_t buffer_size);
void detect_voice_1(int16_t *buffer, uint32_t buffer_size );

#endif //_DETECT_HPP
