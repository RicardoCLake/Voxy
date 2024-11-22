#include "processing.h"
#include "ThisThread.h"
#include "fft.h"  // Assuming this library has the necessary FFT functionality
#include "micro.h"
#include <cstdio>

std::complex<float>* samples_array = (std::complex<float>*)calloc(TARGET_AUDIO_BUFFER_NB_SAMPLES, sizeof(std::complex<float>));
std::vector<float> magnitude(TARGET_AUDIO_BUFFER_NB_SAMPLES / 2);
std::vector<float> variables(9);

// Calculates the magnitude of each FFT component
void getMagnitude(std::complex<float> *samples_array, int size_array) {
    // Calculate the magnitude for each frequency bin
    for (size_t i = 0; i < size_array / 2; ++i) {
        magnitude[i] = std::abs(samples_array[i]);
    }

}

// Applies Mel filters to the magnitude and calculates the Mel factors
void applyMelFilters() {
    std::vector<float> mel_factors(mel_filters.size(), 0.0f);
    for (size_t i = 0; i < mel_filters.size(); ++i) {
        for (size_t j = 0; j < magnitude.size(); ++j) {
            // Adjust index to match the size of the magnitude and the Mel filter
            size_t mel_index = (j * mel_filters[i].size()) / magnitude.size();
            mel_factors[i] += magnitude[j] * mel_filters[i][mel_index];
        }
    }
    for (size_t i = 0; i < 6; ++i) {
        variables[i] = mel_factors[i];
    }
    
}

// Processes an int16_t array, applies FFT, calculates magnitude, and returns Mel coefficients
void wholeProcessing(int16_t* array, size_t size_array) {
    
    
    // Step 1: Convert int16_t input array to complex<float>
    for (size_t i = 0; i < size_array; ++i) {
        samples_array[i] = std::complex<float>(array[i], 0.0f);
    }

    // Step 2: Apply FFT
    FFT(samples_array);

    // Step 3: Calculate the magnitude
    getMagnitude(samples_array, size_array);

}

// Prints Mel factors in CSV format (comma-separated)
void printVariablesCSV() {
    for (size_t i = 0; i < variables.size(); ++i) {
        if (i > 0) {
            printf(", ");
        }
        printf("%f", variables[i]);
    }
    
}

void getTopFrequencies() {
    // Create a vector of indices
    std::vector<int> indices(magnitude.size());
    for (int i = 0; i < indices.size(); ++i) {
        indices[i] = i;
    }

    // Sort indices based on the corresponding values in the magnitudes vector
    std::sort(indices.begin(), indices.end(), [&](int a, int b) {
        return magnitude[a] > magnitude[b];
    });

    for (size_t i = 0; i < 3; ++i) {
        variables[i+6] = indices[i];
    }

/*
    // Find the index of the maximum magnitude
    int max_index = std::distance(magnitude.begin(), std::max_element(magnitude.begin(), magnitude.end()));
    // Print the top three frequencies separated by commas
    printf("%d", max_index);
*/


}

// Prints the FFT magnitudes in a comma-separated format
void printFFTMagnitudes() {
    // Print each magnitude separated by commas
    for (size_t i = 0; i < magnitude.size(); ++i) {
        printf("%f", magnitude[i]);
        ThisThread::sleep_for(100ms);
        if (i < magnitude.size() - 1) {
            printf(", ");
        }
    }
    printf("\n");  // Newline at the end
}

// Logistic regression coefficients from Python model
const std::vector<float> coefficients = {
    -1.71528748e-04,  1.59865366e-04, -4.56487447e-05,  6.95402580e-05,
    2.64418335e-05, -2.78749501e-04, -4.05218606e-11, -1.83194672e-10,
    -7.28887183e-09
};

// Intercept (bias term) from Python model
const float intercept = -4.96123615e-09;

// Sigmoid function
float sigmoid(float z) {
    return 1.0f / (1.0f + expf(-z));
}

// Logistic regression function for classification
int classifyLogisticRegression() {
    // Ensure input size matches coefficients size
    if (variables.size() != coefficients.size()) {
        printf("Error: Variables size does not match coefficients size.\n");
        return -1;
    }

    // Calculate the linear combination of inputs and coefficients
    float linear_combination = intercept;
    for (size_t i = 0; i < variables.size(); ++i) {
        linear_combination += variables[i] * coefficients[i];
    }

    // Apply the sigmoid function to get the probability
    float probability = sigmoid(linear_combination);

    // Return 1 if probability >= 0.5, otherwise return 0
    return (probability >= 0.5) ? 1 : 0;
}