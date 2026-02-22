#pragma once
#include <dsp/transform_functions.h>
#include "../include/includes.h"

struct Bin {
	int index;
	float magnitude;
	// float freqency;
};

void init_dsp();
void fft();
void sort_bins_by_magnitude(Bin *bins);
void sort_bins_by_index(Bin *bins, uint16_t start, uint16_t end);
void normalize_magnitudes(uint16_t size);

extern float fft_input[FFT_SIZE];
extern float fft_output[FFT_SIZE];
extern float magnitude_buffer[FFT_SIZE / 2+1];

