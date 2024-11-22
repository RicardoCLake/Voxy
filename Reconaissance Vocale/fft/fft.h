/**
 * @file fft.h
 * @author Modar Nasser
 * 
 * @copyright File under MIT license
 * 
 */

#ifndef FFT_H
#define FFT_H

#include <complex>
#include "micro.h"

#define PI 3.1415926535

// fréquences audible jusque 20kHz, on prend le double pour la fréquence d'échantillonage
const int FFT_SAMPLING_FREQ = 16000;

// fft sur 2^10 échantillons
const int FFT_SAMPLES_NB = TARGET_AUDIO_BUFFER_NB_SAMPLES;

// remplit W en calculant les twiddles factors
void initTwiddleFactors();

// calcul la FFT sur le tableau d'échantillon donné
void FFT(std::complex<float>* data_array);

#endif
