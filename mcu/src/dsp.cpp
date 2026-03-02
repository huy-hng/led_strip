#include <algorithm>

#include "../include/dsp.h"
#include "../include/adc.h"

float window[FFT_SIZE];
float fft_input[FFT_SIZE];		  // input (real)
float fft_output[FFT_SIZE];		  // complex output (interleaved)
float magnitude_buffer[NUM_BINS]; // usable bins

arm_rfft_fast_instance_f32 fft_instance;

void init_dsp() {
	// init window
	for (int i = 0; i < FFT_SIZE; i++)
		window[i] = 0.5f * (1.0f - cosf(2.0f * PI * i / (FFT_SIZE - 1)));

	arm_rfft_fast_init_f32(&fft_instance, FFT_SIZE);
}

void normalize_magnitudes(uint16_t size) {
	size = size == 0 ? NUM_BINS - 1 : size;
	float db_normalizer = 20.0f * log10f(FFT_SIZE / 2.0);
	for (int i = 1; i <= size; i++) {
		float db = 20.0f * log10f(magnitude_buffer[i] + EPSILON);
		db -= db_normalizer;

		// clamp
		if (db < DB_MIN)
			db = DB_MIN;

		// normalize between 0..1
		magnitude_buffer[i] = (db - DB_MIN) / (-DB_MIN);
	}
}

static uint8_t step = 0;
static uint16_t dc_offset = DEFAULT_DC_OFFSET;
void fft() {
	if (step++ == 0)
		dc_offset = calc_dc_offset(fft_input, FFT_SIZE);

	float volume = 0;
	for (int i = 0; i < FFT_SIZE; i++) {
		fft_input[i] -= dc_offset;
		volume += fabsf(fft_input[i]);
		fft_input[i] /= dc_offset; // normalized between -1...1
		fft_input[i] *= window[i]; // apply window
	}

	arm_rfft_fast_f32(&fft_instance, fft_input, fft_output, 0);

	// calculate the magnitude at each bin
	arm_cmplx_mag_f32(&fft_output[2],		// skip first two special entries
					  &magnitude_buffer[1], // bin 1 onward
					  NUM_BINS - 2);

	// magnitude_buffer[0] = fabsf(fft_output[0]) / FFT_SIZE;			  // DC
	magnitude_buffer[0] = volume / (FFT_SIZE * dc_offset);			  // volume
	magnitude_buffer[FFT_SIZE / 2] = fabsf(fft_output[1]) / FFT_SIZE; // Nyquist
}
