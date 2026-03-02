#pragma once
#include <dsp/transform_functions.h>
#include "../include/includes.h"


#define DB_MIN -60.0f // dynamic range floor
#define NUM_BINS (FFT_SIZE / 2 + 1)
#define FREQUENCY_PER_BIN = (SAMPLE_RATE / FFT_SIZE);

struct Bin {
	int index;
	float magnitude;
	// float freqency;
};

void init_dsp();
void fft(float *input, float *output);
void normalize_magnitudes(float *magnitudes);
void normalize_magnitudes(float *magnitudes, uint16_t size);
int find_peaks_simple(const float *x, int n, int *peaks_out, int max_peaks, int min_distance);
void filter_peaks(float *mags, uint32_t amount);

extern float fft_input[FFT_SIZE];
extern float fft_bass_input[FFT_SIZE];
extern float magnitude_buffer[NUM_BINS];
extern float magnitude_bass_buffer[NUM_BINS];
