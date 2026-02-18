#include <stdio.h>
#include <pico/time.h>
#include "hardware/adc.h"

#include "../include/pins.h"
#include "../include/dma.h"
#include "../include/adc.h"
#include "../include/util.h"
#include "../include/visualizer.h"


void init() {
	cyw43_arch_init();
	stdio_init_all(); // Initialize UART/USB stdio

	for (int i = 2; i > 0; i--) {
		printf("Starting in %d...\n", i);
		sleep_ms(1000);
	}

	init_utils();
	init_adc();
	init_dma();
	adc_run(true);
}

int step = 0;
void loop() {
	if (adc_buffer_ready) {
		int vol = mean(ADC_BUF_LEN, adc_buffer);
		adc_buffer_ready = false;
		print_volume(vol);
		sleep_ms(50);

		step++;
	}
}

int main() {
	init();
	while (true)
		loop();
}
