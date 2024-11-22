#include "stm32l475e_iot01_audio.h"
#include "mbed.h"
#include <cstdint>


#ifndef _MICRO_HPP
#define _MICRO_HPP

    extern BSP_AUDIO_Init_t MicParams; 
    #define TARGET_AUDIO_BUFFER_NB_SAMPLES 1024 // 2.5 seconds is the maximum (*5/2)
    extern int16_t *target_audio_buffer ;
    extern size_t target_audio_buffer_ix ;
    extern int8_t PCM_Buffer[PCM_BUFFER_LEN];
    extern size_t transfer_events_after_button;

#endif //_MICRO_HPP