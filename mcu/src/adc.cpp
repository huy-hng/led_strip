#include "../include/adc.h"

void init_adc() {
	adc_init();
	adc_gpio_init(PIN_ADC0);
	// adc_gpio_init(PIN_ADC1);
	adc_select_input(0);

	adc_fifo_setup(true,   // Write each completed conversion to FIFO
				   true,   // Enable DMA request (DREQ) when FIFO contains data
				   1,	   // DREQ when at least 1 sample present
				   false,  // If enabled, bit 15 of the FIFO contains error flag for each sample
				   false); // No byte shift (keep 12-bit)
						   // Shift FIFO contents to be one byte in size (for byte DMA) - enables
						   // DMA to byte buffers.

	// switch between ADC0 and ADC1 to get left and right audio channels
	// every bit corresponds to a channel -> right most bit = 1 means that channel will be sampled
	// 0b0110 means channel 1 and 2 will be sampled
	// adc_set_round_robin(0b0011);

	// sample_rate = 48MHz / (clkdiv + 1)
	uint16_t clkdiv = SAMPLE_RATE > 0 ? (48e6 / (SAMPLE_RATE)) - 1 : 0;
	adc_set_clkdiv(clkdiv);

	adc_run(false);
	adc_fifo_drain();
}
