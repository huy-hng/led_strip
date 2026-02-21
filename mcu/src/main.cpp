#include "../include/includes.h"
#include "../include/dsp.h"
#include "../include/dma.h"
#include "../include/adc.h"
#include "../include/visualizer.h"

void countdown() {
	for (int i = 3; i > 0; i--) {
		printf("Starting in %d...\n", i);
		sleep_ms(1000);
	}
}

void init() {
	cyw43_arch_init();
	stdio_init_all(); // Initialize UART/USB stdio

	countdown(); // must be before the other inits and must be after stdio init

	init_utils();
	init_adc();
	init_dma();
	init_dsp();
	adc_run(true);
}

void loop() {
	uint16_t vol = adc_read();
	vol = abs(vol - 2048) * 2;
	print_volume(vol);
	printr("%4d", vol);
	sleep_ms(10);
}

uint16_t adc_view[FFT_SIZE];
uint8_t current_index = 0;
static uint64_t start_time = 0;
void core1_entry() {
	while (true) {
		// println("in core1");
		printf("%llu\n", millis() - start_time);
		start_time = millis();

		uint8_t current_index = multicore_fifo_pop_blocking();
		fetch_frame_via_ptrs(adc_view, current_index);
		int vol = mean(FFT_SIZE, adc_view);
		vol = abs(vol - 2048) * 2;
		print_volume(vol);
		printr("%4d", vol);
		read_index = current_index;
	}
}

int main() {
	init();

	multicore_launch_core1(fft_loop);
	// multicore_launch_core1(core1_entry);

	while (true) {
		tight_loop_contents();
		// printf("a");
		// multicore_fifo_push_blocking(vol);
		// loop();
		sleep_ms(1000);
	}
}
