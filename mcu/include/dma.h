#pragma once
#include <cmath>
#include "../config.h"
#include "pico/stdlib.h"

#define ADC_BUF_LEN		FFT_WINDOW_SIZE * 2	// Must be power of two
#define ADC_BUF_BYTES	ADC_BUF_LEN * sizeof(uint16_t)

const uint8_t ADC_BUF_EXPONENT = log2(ADC_BUF_BYTES); // depends on above: 2^12 = 2048 samples

__attribute__((aligned(ADC_BUF_BYTES))) // Must align buffer to ring size in bytes

extern volatile uint16_t adc_buffer[ADC_BUF_LEN];
extern volatile uint16_t *adc_buffer_view;
extern volatile bool adc_buffer_ready;

void init_dma();
void init_dma();


extern int dma_chan;
