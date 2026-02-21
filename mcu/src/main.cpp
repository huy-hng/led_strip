#include "../include/includes.h"
#include "../include/dsp.h"
#include "../include/dma.h"
#include "../include/adc.h"
#include "../include/visualizer.h"

void init() {
	cyw43_arch_init();
	stdio_init_all(); // Initialize UART/USB stdio

	for (int i = 3; i > 0; i--) {
		printf("Starting in %d...\n", i);
		sleep_ms(1000);
	}

	init_utils();
	init_adc();
	init_dma();
	adc_run(true);
}

int step = 0;
// static uint64_t start_time = 0;
// uint8_t last_index = 0;
// volatile uint16_t adc_view[FFT_WINDOW_SIZE];

void loop() {
	// if (write_index == last_index) return;
	// last_index = write_index;

	// start_time = millis();
	// printf("%llu\n", millis() - start_time);


	// fetch_frame_via_ptrs(adc_view, last_index);
	// int vol = mean(ADC_BUF_LEN, adc_view);
	// print_volume(vol);

	// read_index = last_index;

	// sleep_ms(10);
	// step++;
}

int main() {
	init();
	// fft_loop();

	multicore_launch_core1(fft_loop);
	while (true) {
		tight_loop_contents();
		// loop();
		// sleep_ms(500);
	}

}
