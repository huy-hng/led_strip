#pragma once
#include "../include/includes.h"

void init_adc();

const uint16_t DEFAULT_DC_OFFSET = 2079;
// static float dc_offset = default_offset;

template <typename T>
uint16_t calc_dc_offset(T *arr, int size) {
	float avg = 0;
	for (int i = 0; i < size; i++)
		avg += arr[i];

	avg /= size;

	if (avg == 0) return DEFAULT_DC_OFFSET;
	return (uint16_t) avg;
}

template <typename T>
void apply_dc_offset(T *arr, uint16_t size, uint16_t offset) {
	offset = offset == 0 ? DEFAULT_DC_OFFSET : offset;
	for (int i = 0; i < size; i++)
		arr[i] -= offset;

	// fft_input[i] /= dc_offset;
	// fft_input[i] = (fft_input[i] - dc_offset) / dc_offset;
	// fft_input[i] = (fft_input[i] - dc_offset) * window[i];
}

void apply_dc_offset(int16_t *arr, uint16_t size, uint16_t offset);
