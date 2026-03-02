#include <algorithm>

#include "../include/dsp.h"
#include "../include/adc.h"

float window[FFT_SIZE];
float fft_input[FFT_SIZE]; // input (real)
float fft_bass_input[FFT_SIZE];
float magnitude_buffer[NUM_BINS];	   // usable bins
float magnitude_bass_buffer[NUM_BINS]; // usable bins

arm_rfft_fast_instance_f32 fft_instance;

void init_dsp() {
	// init window
	for (int i = 0; i < FFT_SIZE; i++)
		window[i] = 0.5f * (1.0f - cosf(2.0f * PI * i / (FFT_SIZE - 1)));

	arm_rfft_fast_init_f32(&fft_instance, FFT_SIZE);
}

void normalize_magnitudes(float *magnitudes) { normalize_magnitudes(magnitudes, 0); }

void normalize_magnitudes(float *magnitudes, uint16_t size) {
	// float db_avg = -40;
	float db_avg = 2 * 20.0f * log10f(magnitudes[0]);
	db_avg = db_avg < DB_MIN ? DB_MIN : db_avg;

	size = size == 0 ? NUM_BINS - 1 : size;
	float db_normalizer = 20.0f * log10f(FFT_SIZE / 2.0);
	for (int i = 1; i <= size; i++) {
		float db = 20.0f * log10f(magnitudes[i] + EPSILON);
		db -= db_normalizer;

		// clamp
		if (db < db_avg)
			db = db_avg;

		// normalize between 0..1
		magnitudes[i] = (db - db_avg) / (-db_avg);
	}
}

static uint8_t step = 0;
static uint16_t dc_offset = DEFAULT_DC_OFFSET;
void fft(float *input, float *output) {
	float fft_output[FFT_SIZE]; // complex fft_output (interleaved)

	if (++step == 0)
		dc_offset = calc_dc_offset(input, FFT_SIZE);

	float volume = 0;
	for (int i = 0; i < FFT_SIZE; i++) {
		input[i] -= dc_offset;
		volume += fabsf(input[i]);
		input[i] /= dc_offset; // normalized between -1...1
		input[i] *= window[i]; // apply window
	}

	arm_rfft_fast_f32(&fft_instance, input, fft_output, 0);

	// calculate the magnitude at each bin
	arm_cmplx_mag_f32(&fft_output[2], // skip first two special entries
					  &output[1],	  // bin 1 onward
					  NUM_BINS - 2);

	// output[0] = fabsf(fft_output[0]) / FFT_SIZE;			  // DC
	output[0] = volume / (FFT_SIZE * dc_offset);			// volume
	output[FFT_SIZE / 2] = fabsf(fft_output[1]) / FFT_SIZE; // Nyquist
}

// Finds peaks in a signal x[0..n-1], returns count and writes indices into peaks_out (preallocated)
int find_peaks_simple(const float *x, int n, int *peaks_out, int max_peaks, int min_distance) {
	int count = 0;

	// Step 1: find local maxima (including flat peak midpoints)
	for (int i = 1; i < n - 1; i++) {
		if (x[i] > x[i - 1] && x[i] > x[i + 1]) {
			// Strict local max
			if (count < max_peaks) {
				peaks_out[count++] = i;
			}
		} else if (x[i] == x[i - 1] && x[i] == x[i + 1]) {
			// Potential plateau
			int left = i - 1;
			while (left > 0 && x[left] == x[i])
				left--;
			int right = i + 1;
			while (right < n - 1 && x[right] == x[i])
				right++;

			// If plateau is a flat maximum
			if (x[left] < x[i] && x[right] < x[i]) {
				int mid = (left + 1 + right - 1) / 2; // middle of plateau
				if (mid == i) {						  // only record once
					if (count < max_peaks) {
						peaks_out[count++] = mid;
					}
				}
			}
			i = right; // skip plateau
		}
	}

	// Step 2: enforce minimum distance (descending order by height)
	// simple greedy filter: keep highest peaks first
	for (int i = 0; i < count; i++) {
		for (int j = i + 1; j < count; j++) {
			if (abs(peaks_out[i] - peaks_out[j]) < min_distance) {
				// whoever has lower value gets invalidated
				if (x[peaks_out[i]] >= x[peaks_out[j]]) {
					peaks_out[j] = -1;
				} else {
					peaks_out[i] = -1;
				}
			}
		}
	}

	// Compact results
	int write = 0;
	for (int i = 0; i < count; i++) {
		if (peaks_out[i] >= 0) {
			peaks_out[write++] = peaks_out[i];
		}
	}
	return write;
}

void filter_peaks(float *mags, uint32_t amount) {
	int num_peaks = 20;
	int peaks[num_peaks];
	int found_peaks = find_peaks_simple(mags, amount, peaks, num_peaks, 8);

	int pos = 0;
	for (int i = 0; i < amount; i++) {
		if (peaks[pos] == i)
			mags[i] *= 8;
		else
			mags[i] = 0;

		if (i == found_peaks)
			break;
	}
}

int find_peaks_usage(void) {
	float signal[] = {1, 3, 7, 6, 4, 5, 4, 2, 8, 6};
	int n = sizeof(signal) / sizeof(signal[0]);
	int peaks[20];

	int num_peaks = find_peaks_simple(signal, n, peaks, 20, 2);

	printf("Peaks at indices: ");
	for (int i = 0; i < num_peaks; i++) {
		printf("%d ", peaks[i]);
	}
	printf("\n");

	return 0;
}
