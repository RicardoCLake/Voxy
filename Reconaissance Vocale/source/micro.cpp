#include "micro.h"
#include "detect.h"
#include "main.h"

// Driver utilities
int8_t PCM_Buffer[PCM_BUFFER_LEN]; //PCM_BUFFER_LEN is defined by the driver as 64 (bytes)
BSP_AUDIO_Init_t MicParams;

// Our place to store final audio (alloc on the heap)
// (the samples audio are with 16 bits)

int16_t *target_audio_buffer = (int16_t*)calloc(TARGET_AUDIO_BUFFER_NB_SAMPLES, sizeof(int16_t));
size_t target_audio_buffer_ix = 0;

// Skip the first 100 events (100 ms.) to not record the button click
static size_t SKIP_FIRST_EVENTS = 100;
size_t transfer_events_after_button = 0;


/**
* @brief  Half Transfer user callback, called by BSP functions.
* @param  None
* @retval None
*/
void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance) {
    transfer_events_after_button++;
    if (transfer_events_after_button < SKIP_FIRST_EVENTS) return;

    uint32_t buffer_size = PCM_BUFFER_LEN / 2; /* Half Transfer */
    uint32_t nb_samples = buffer_size / sizeof(int16_t); /* convertion from Bytes to samples of 16 bits */

    if ((target_audio_buffer_ix + nb_samples) > TARGET_AUDIO_BUFFER_NB_SAMPLES) {
        return;
    }

    /* Copy first half of PCM_Buffer from Microphones onto Fill_Buffer */
    memcpy(((uint8_t*)target_audio_buffer) + (target_audio_buffer_ix * 2), PCM_Buffer, buffer_size);

    // Detect the voice
    detect_voice_1(target_audio_buffer + target_audio_buffer_ix, nb_samples);

    // Update index
    target_audio_buffer_ix += nb_samples;

    if (target_audio_buffer_ix >= TARGET_AUDIO_BUFFER_NB_SAMPLES) {
        ev_queue.call(&target_audio_buffer_full);
        return;
    }
}

/**
* @brief  Transfer Complete user callback, called by BSP functions.
* @param  None
* @retval None
*/
void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance) {
    transfer_events_after_button++;
    if (transfer_events_after_button < SKIP_FIRST_EVENTS) return;

    uint32_t buffer_size = PCM_BUFFER_LEN / 2; /* Half Transfer */
    uint32_t nb_samples = buffer_size / sizeof(int16_t); /* Bytes to Length */

    if ((target_audio_buffer_ix + nb_samples) > TARGET_AUDIO_BUFFER_NB_SAMPLES) {
        return;
    }

    /* Copy second half of PCM_Buffer from Microphones onto Fill_Buffer */
    memcpy(((uint8_t*)target_audio_buffer) + (target_audio_buffer_ix * 2),
        ((uint8_t*)PCM_Buffer) + (nb_samples * 2), buffer_size);

    // Detect the voice
    detect_voice_1(target_audio_buffer + target_audio_buffer_ix, nb_samples);

    // Update index
    if(SAVING_AUDIO) {
        target_audio_buffer_ix += nb_samples;
    } else {
        target_audio_buffer_ix = 0;
    }

    if (target_audio_buffer_ix >= TARGET_AUDIO_BUFFER_NB_SAMPLES) {
        ev_queue.call(&target_audio_buffer_full);
        return;
    }
}

/**
  * @brief  Manages the BSP audio in error event.
  * @param  Instance Audio in instance.
  * @retval None.
  */
void BSP_AUDIO_IN_Error_CallBack(uint32_t Instance) {
    printf("BSP_AUDIO_IN_Error_CallBack\n");
}

