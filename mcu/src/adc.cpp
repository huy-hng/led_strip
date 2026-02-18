#pragma once
#include <pico/time.h>
#include "hardware/adc.h"
#include "../config.h"
#include "../include/pins.h"
#include "../include/adc.h"

#define ADC_CHANNEL 0

void init_adc() {
	adc_init();
	adc_gpio_init(PIN_ADC0); // GPIO26 = ADC0
	adc_select_input(ADC_CHANNEL);

	adc_fifo_setup(true,   // Write each completed conversion to FIFO
				true,   // Enable DMA request (DREQ)
				1,	   // DREQ when at least 1 sample present
				false,  // No error bit
				false); // No byte shift (keep 12-bit)

	// switch between ADC0 and ADC1 to get left and right audio channels
	// adc_set_round_robin(0b0011);

	// sample_rate = 48MHz / (clkdiv + 1)
	uint16_t clkdiv = SAMPLE_RATE > 0 ? (48e6 / SAMPLE_RATE) - 1 : 0;
	sleep_ms(2000);
	adc_set_clkdiv(clkdiv);

	adc_run(false);
	adc_fifo_drain();
}
