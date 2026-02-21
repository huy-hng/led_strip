#include <algorithm>
#include <dsp/transform_functions.h>

#include "../include/includes.h"
#include "../include/dsp.h"
#include "../include/dma.h"

float window[FFT_SIZE];
float fft_input[FFT_SIZE];				  // input (real)
float fft_output[FFT_SIZE];				  // complex output (interleaved)
float magnitude_buffer[FFT_SIZE / 2 + 1]; // usable bins

arm_rfft_fast_instance_f32 fft_instance;

struct Bin {
	int index;
	// float freqency;
	float magnitude;
};

const float default_offset = 2069;
static float dc_offset = default_offset;

void init_dsp() {
	// init window
	for (int i = 0; i < FFT_SIZE; i++)
		window[i] = 0.5f * (1 - cosf(2 * PI * i / (FFT_SIZE - 1)));

	arm_rfft_fast_init_f32(&fft_instance, FFT_SIZE);
}

void apply_window(float *samples) {
	for (int i = 0; i < FFT_SIZE; i++)
		samples[i] *= window[i];
}

void calc_dc_offset() {
	float avg = 0.0f;

	for (int i = 0; i < FFT_SIZE; i++)
		avg += fft_input[i];

	avg /= FFT_SIZE;

	dc_offset = avg == 0 ? default_offset : avg;
}

void apply_dc_offset() {
	for (int i = 0; i < FFT_SIZE; i++) {
		// fft_input[i] = (fft_input[i] - dc_offset) * window[i];
		fft_input[i] -= dc_offset;
		// fft_input[i] /= dc_offset;
		// fft_input[i] = (fft_input[i] - dc_offset) / dc_offset;
	}
}

void print_sorted_bin_magnitudes() {
	Bin bins[FFT_SIZE / 2 + 1];
	for (int i = 0; i <= FFT_SIZE / 2; i++)
		bins[i] = {i, magnitude_buffer[i] / ((float)FFT_SIZE / 2)};

	std::sort(bins, bins + FFT_SIZE / 2 + 1, [](const Bin &a, const Bin &b) { //
		return a.magnitude > b.magnitude;
	});

	float frequency_per_bin = (float)SAMPLE_RATE / FFT_SIZE;
	for (int i = 0; i < 12; i++) {
		// printf("%2.0f ", bins[i]);
		printf("%3d=%8.0f | ", bins[i].index, bins[i].magnitude);
		// printf("%3d=%8.1f | ", bins[i].index, bins[i].magnitude);
		// printf("%.0f:%.0f | ", bins[i][0], bins[i][1]);
	}
	println("");
}

static uint8_t step = 0;
void fft() {
	if (step++ == 0)
		calc_dc_offset();

	// apply_window(fft_input);

	apply_dc_offset();
	arm_rfft_fast_f32(&fft_instance, fft_input, fft_output, 0);

	// // magnitude_buffer size = FFT_SIZE/2 + 1 = 513
	// magnitude_buffer[0] = fabsf(fft_output[0]);			   // DC
	// magnitude_buffer[FFT_SIZE / 2] = fabsf(fft_output[1]); // Nyquist
	// for (int k = 1; k < FFT_SIZE / 2; k++) {
	// 	float real = fft_output[2 * k];
	// 	float imag = fft_output[2 * k + 1];
	// 	magnitude_buffer[k] = sqrtf(real * real + imag * imag);
	// 	// scale magnitude between 0 and 1
	// 	// float32_t scaled_mag = (mag * 2 / (MAX_INT16 * FFTLEN));
	// }

	// calculate the magnitude at each bin
	arm_cmplx_mag_f32(fft_output, magnitude_buffer, FFT_SIZE / 2);
	// arm_cmplx_mag_f32(&fft_output[2],		// skip first two special entries
	// 				  &magnitude_buffer[1], // bin 1 onward
	// 				  (FFT_SIZE / 2) - 1);

	print_sorted_bin_magnitudes();
}

void fft_loop() {
	while (true) {
		uint8_t current_index = multicore_fifo_pop_blocking();
		fetch_frame_via_ptrs(fft_input, current_index);
		fft();
		read_index = current_index;
	}
}
