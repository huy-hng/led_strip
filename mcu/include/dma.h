#pragma once
#include "../include/includes.h"

#define ADC_BUF_LEN (FFT_WINDOW_SIZE * 2) // Must be power of two
#define ADC_BUF_BYTES (ADC_BUF_LEN * sizeof(uint16_t))
#define NUM_FRAMES (ADC_BUF_LEN / FFT_HOP_SIZE)

const uint8_t ADC_BUF_EXPONENT = log2(ADC_BUF_BYTES); // depends on above: 2^12 = 2048 samples

typedef struct {
	volatile uint16_t *chunks[2]; // up to 2 contiguous slices
	uint32_t lengths[2];		  // number of samples in each slice
	uint32_t num_chunks;		  // 1 or 2
} fft_frame_ptrs_t;

extern fft_frame_ptrs_t frame_ptrs[NUM_FRAMES];

__attribute__((aligned(ADC_BUF_BYTES))) // Must align buffer to ring size in bytes

extern volatile uint16_t adc_buffer[ADC_BUF_LEN];
extern volatile uint8_t write_index;
extern volatile uint8_t read_index;

void init_dma();

// template <typename T>
// extern void fetch_frame_via_ptrs(T buffer, uint8_t frame_id);

extern int dma_chan;
