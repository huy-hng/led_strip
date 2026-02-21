#include <dsp/transform_functions.h>

#include "../include/includes.h"
#include "../include/dsp.h"
#include "../include/dma.h"

float fft_input[FFT_WINDOW_SIZE];
float fft_output[FFT_WINDOW_SIZE];
float magnitude_buffer[FFT_WINDOW_SIZE];



float window[FFT_WINDOW_SIZE];
arm_rfft_fast_instance_f32 fft_instance;

void init_dsp() {
	// init window
	for (int i = 0; i < FFT_WINDOW_SIZE; i++)
		window[i] = 0.5f * (1 - cosf(2 * PI * i / (FFT_WINDOW_SIZE - 1)));

	arm_rfft_fast_init_f32(&fft_instance, FFT_WINDOW_SIZE);
}

void apply_window(float *data) {
	for (int i = 0; i < FFT_WINDOW_SIZE; i++)
		data[i] *= window[i];
}

void fft() {
	apply_window(fft_input);
	arm_rfft_fast_f32(&fft_instance, fft_input, fft_input, 0);
	arm_cmplx_mag_f32(fft_input, magnitude_buffer, FFT_WINDOW_SIZE / 2);
}

template <typename T>
void fetch_frame_via_ptrs(T *buffer, uint8_t frame_id) {
	fft_frame_ptrs_t *frame = &frame_ptrs[frame_id];
	uint32_t pos = 0;

	for (uint32_t c = 0; c < frame->num_chunks; c++) {
		for (uint32_t i = 0; i < frame->lengths[c]; i++) {
			buffer[pos++] = (T)frame->chunks[c][i];
		}
	}
}

uint16_t adc_view[FFT_WINDOW_SIZE];
static uint64_t start_time = 0;
uint8_t current_index = 0;

void fft_loop() {
	static uint64_t start_time = 0;
	while (true) {

		println("fft loop time: %llu", millis() - start_time);
		start_time = millis();

		uint8_t current_index = multicore_fifo_pop_blocking();
		fetch_frame_via_ptrs(fft_input, current_index);
		fft();

		// current_index = write_index;
		// fetch_frame_via_ptrs(adc_view, current_index);
		// int vol = mean(ADC_BUF_LEN, adc_view);
		// print_volume(vol);
		// sleep_ms(10);

		read_index = current_index;
	}
}
