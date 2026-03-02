#include "../include/includes.h"
#include "../include/dma.h"
#include "../include/adc.h"
#include "../include/visualizer.h"
#include "../include/note_detection.h"
#include "../include/dsp.h"

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

	init_visualizer();
	init_adc();
	init_dma();
	init_dsp();
	init_notes();
	adc_run(true);
}

void loop() {
	uint16_t vol = adc_read();
	vol = abs(vol - 2048) * 2;
	print_volume(vol);
	printr("%4d", vol);
	sleep_ms(10);
}

const uint16_t view_size = (FFT_HOP_SIZE * 4);
const uint16_t view_start = FFT_SIZE - view_size;
int16_t adc_view[FFT_SIZE];

void visualize_volume(uint8_t dma_index) {
	fetch_frame_via_ptrs(adc_view, dma_index);
	// uint16_t offset = calc_dc_offset(adc_view, view_size);

	uint16_t offset = calc_dc_offset(adc_view, FFT_SIZE);
	apply_dc_offset(&adc_view[view_start], view_size, offset);
	float vol = positive_mean(view_size, &adc_view[view_start]);
	vol /= offset;
	// create_vertical_bar(vol);
	// print_volume(vol);
	printr("%f", vol);

	sleep_ms(8);
}

void core1_entry() {
	static uint64_t start_time = 0;
	uint8_t current_index = 0;

	while (true) {
		// printf("%llu\n", millis() - start_time);
		// start_time = millis();
		uint8_t current_index = multicore_fifo_pop_blocking();
		fetch_frame_via_ptrs(fft_input, current_index);

		// visualize_volume(current_index);
		// read_index = current_index;
		// continue;

		fft();
		normalize_magnitudes(0);
		float *mags = compute_note_scores(magnitude_buffer);
		print_notes(mags);

		// normalize_magnitudes(200);
		// normalized_bands();
		// print_bands();
		// visualize_fft_horizontal();
		
		// for (int i = 0; i < 55; i++) {
		// 	printf("%5.2f ", magnitude_buffer[i]);
		// }
		// println("");

		// sleep_ms(1);
		read_index = current_index;
	}
}

int main() {
	init();
	multicore_launch_core1(core1_entry);
	while (true) {
		// loop();
		sleep_ms(1000);
	}
}
