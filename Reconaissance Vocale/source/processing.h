#ifndef MEL_FILTER_BANK_H
#define MEL_FILTER_BANK_H

#include <vector>
#include <complex>
#include <cstdint>
#include <algorithm>
#include <cmath>
#include "mel_filters.h"  // Includes Mel filters loaded from JSON or defined manually

extern std::complex<float>* samples_array;

// Calculates the magnitude from an array of complex samples (FFT)
void getMagnitude(const std::vector<std::complex<float>>& samples_array);

// Applies Mel filters to the FFT magnitude and returns the resulting factors
void applyMelFilters();

// Processes an int16_t array by applying FFT, calculating the magnitude, and returning Mel coefficients
void wholeProcessing(int16_t* array, size_t size_array);

// Prints Mel factors in CSV format (one line, comma-separated)
void printVariablesCSV();

void getTopFrequencies();

// Prints the FFT magnitudes in a comma-separated format
void printFFTMagnitudes();

// Logistic regression function for classification
int classifyLogisticRegression();

#endif // MEL_FILTER_BANK_H
