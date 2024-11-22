#include "mbed.h"
#include "stm32l475e_iot01_audio.h"
#include <cstdint>

#include "detect.h"
#include "micro.h"
#include "processing.h"


#ifndef _MAIN_HPP
#define _MAIN_HPP

    extern EventQueue ev_queue;
    extern volatile bool SAVING_AUDIO;

    // Functions
    void saving_condition_monitor();
    void button_callback();
    void print_power();
    void generate_wav_file();
    void target_audio_buffer_full();
    void start_recording();

#endif //_MAIN_HPP