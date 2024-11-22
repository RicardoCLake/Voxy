#include "main.h"
#include "processing.h"
#include <cstdio>
#include "fft.h"
#include "connexion.h"

// Output led
static DigitalOut led2(LED2);

// To button interruption
EventQueue ev_queue;

// Saving audio flag
volatile bool SAVING_AUDIO = false;

const bool VERBOSE = true;
const bool VERBOSE_MEL = false;
const bool VERBOSE_FFT = false;
const bool WIFI_MODE = false;

int count_of_samples = 0;

// Function to monitor the condition in a separate thread
void saving_condition_monitor() {
    bool first_time = true;
    while (true) {
        if (SAVING_AUDIO && first_time) {
            if(VERBOSE) printf("OK Audio Saving\n");
            if(VERBOSE) print_power();
            first_time = false; // Optionally reset the condition
        }
        if (!SAVING_AUDIO && !first_time) {
            first_time = true;
        }
        ThisThread::sleep_for(10ms); // Check every 5ms
    }
}

// Callback to print the power
void button_callback() {
    printf("Button pressed\n");
    print_power();
    transfer_events_after_button = 0;
}

void print_power(){
    printf(" --> Power = %llu\n", historical_power);
}

// Generate and print the wav file
void generate_wav_file() {
    // create WAV file
    size_t wavFreq = AUDIO_SAMPLING_FREQUENCY;
    size_t dataSize = (TARGET_AUDIO_BUFFER_NB_SAMPLES * 2);
    size_t fileSize = 44 + (TARGET_AUDIO_BUFFER_NB_SAMPLES * 2);

    unsigned int wav_header[44] = {
        0x52, 0x49, 0x46, 0x46, // RIFF
        fileSize & 0xff, (fileSize >> 8) & 0xff, (fileSize >> 16) & 0xff, (fileSize >> 24) & 0xff,
        0x57, 0x41, 0x56, 0x45, // WAVE
        0x66, 0x6d, 0x74, 0x20, // fmt
        0x10, 0x00, 0x00, 0x00, // length of format data
        0x01, 0x00, // type of format (1=PCM)
        0x01, 0x00, // number of channels
        wavFreq & 0xff, (wavFreq >> 8) & 0xff, (wavFreq >> 16) & 0xff, (wavFreq >> 24) & 0xff,
        0x00, 0x7d, 0x00, 0x00, // 	(Sample Rate * BitsPerSample * Channels) / 8
        0x02, 0x00, 0x10, 0x00,
        0x64, 0x61, 0x74, 0x61, // data
        dataSize & 0xff, (dataSize >> 8) & 0xff, (dataSize >> 16) & 0xff, (dataSize >> 24) & 0xff,
    };

    printf("Total complete events: %u, index is %u\n", transfer_events_after_button, target_audio_buffer_ix);

    // print both the WAV header and the audio buffer in HEX format to serial
    // you can use the script in `hex-to-buffer.js` to make a proper WAV file again
    printf("WAV file:\n");
    for (size_t ix = 0; ix < 44; ix++) {
        printf("%02x", wav_header[ix]);
    }

    uint8_t *buf = (uint8_t*)target_audio_buffer;
    for (size_t ix = 0; ix < TARGET_AUDIO_BUFFER_NB_SAMPLES * 2; ix++) {
        printf("%02x", buf[ix]);
    }
    printf("\n");
}

// Function to print an int16_t array in a comma-separated format
void printInt16Array(const int16_t *array, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        printf("%d", array[i]);  // Print each element as an integer
        ThisThread::sleep_for(100ms);
        if (i < size - 1) {
            printf(", ");
        }
    }
    printf("\n");  // Newline at the end
}

// my callback that gets invoked when TARGET_AUDIO_BUFFER is full
void target_audio_buffer_full() {
    SAVING_AUDIO = false;

    // pause audio stream
    int32_t ret = BSP_AUDIO_IN_Pause(AUDIO_INSTANCE);
    if (ret != BSP_ERROR_NONE) {
        printf("Error Audio Pause (%d)\n", ret);
    }
    else {
        if(VERBOSE) printf("OK Audio Pause\n");
    }

    // print wav file
    //generate_wav_file();

    // Process the input signal and get Mel coefficients
    count_of_samples++;
    wholeProcessing(target_audio_buffer, TARGET_AUDIO_BUFFER_NB_SAMPLES);
    applyMelFilters();
    getTopFrequencies();
    int classification = classifyLogisticRegression();
    if (VERBOSE_MEL) {
        printf("%d,", count_of_samples);
        printVariablesCSV();
        printf("\n");  // Move to the next line after printing all factors

    }
    if(VERBOSE_FFT) {
        //printInt16Array(target_audio_buffer, TARGET_AUDIO_BUFFER_NB_SAMPLES);
        printFFTMagnitudes();
    }
    if (VERBOSE) {
        printf("Classification: %d\n", classification);
    }

    // Leds
    led2 = classification;
    if (WIFI_MODE) send_led_state (classification, VERBOSE);
        
    
    ThisThread::sleep_for(1s);
    historical_power = 0;

    // Restart listening
    start_recording();
}

void start_recording() {
    int32_t ret;
    uint32_t state;

    ret = BSP_AUDIO_IN_GetState(AUDIO_INSTANCE, &state);
    if (ret != BSP_ERROR_NONE) {
        printf("Cannot start recording: Error getting audio state (%d)\n", ret);
        return;
    }
    if (state == AUDIO_IN_STATE_RECORDING) {
        printf("Cannot start recording: Already recording\n");
        return;
    }

    // reset audio buffer location and counters
    target_audio_buffer_ix = 0;


    if (state == AUDIO_IN_STATE_PAUSE) {
        // resume audio stream
        int32_t ret = BSP_AUDIO_IN_Resume(AUDIO_INSTANCE);
        if (ret != BSP_ERROR_NONE) {
            printf("Error Audio Resume (%d)\n", ret);
        }
        else {
            if(VERBOSE) printf("OK Audio Resume\n");
        }
        return;
    }

    ThisThread::sleep_for(1s); //setup time
    ret = BSP_AUDIO_IN_Record(AUDIO_INSTANCE, (uint8_t *) PCM_Buffer, PCM_BUFFER_LEN);
    if (ret != BSP_ERROR_NONE) {
        printf("Error Audio Record (%d)\n", ret);
        return;
    }
    else {
        if(VERBOSE) printf("OK Audio Record\n");
    }

}

int main() {
    if (VERBOSE || VERBOSE_FFT) printf("Hello :)\n");

    if (!target_audio_buffer) {
        printf("Failed to allocate TARGET_AUDIO_BUFFER buffer\n");
        return 0;
    }

    if (!samples_array) {
        printf("Failed to allocate SAMPLES_ARRAY buffer\n");
        return 0;
    }

    // set up the microphone
    MicParams.BitsPerSample = 16;
    MicParams.ChannelsNbr = AUDIO_CHANNELS;
    MicParams.Device = AUDIO_IN_DIGITAL_MIC1;
    MicParams.SampleRate = AUDIO_SAMPLING_FREQUENCY;
    MicParams.Volume = 32;

    int32_t ret = BSP_AUDIO_IN_Init(AUDIO_INSTANCE, &MicParams);

    if (ret != BSP_ERROR_NONE) {
        printf("Error Audio Init (%d)\r\n", ret);
        return 1;
    } else {
        if(VERBOSE) printf("OK Audio Init\t(Audio Freq=%d)\r\n", AUDIO_SAMPLING_FREQUENCY);
    }

    for (int ix = 0; ix < TARGET_AUDIO_BUFFER_NB_SAMPLES;ix++) {
        target_audio_buffer[ix] = 0;
    }

    // Create and start a separate thread to monitor the condition
    Thread thread;
    thread.start(saving_condition_monitor);

    // hit the blue button to record a message
    if (VERBOSE) printf("Press the BLUE button to print the power\n");
    static InterruptIn btn(BUTTON1);
    btn.fall(ev_queue.event(&print_power));

    // mel dataset
    if (VERBOSE_MEL) printf("Count,Mel_1,Mel_2,Mel_3,Mel_4,Mel_5,Mel_6,Top_indice_1,Top_indice_2,Top_indice_3\n");

    // Start recording
    initTwiddleFactors();
    if (WIFI_MODE) init_my_connexion(VERBOSE);
    start_recording();

    ev_queue.dispatch_forever();
}


