#include <dsp/statistics_functions.h>
#include "../include/includes.h"

#include "../include/note_detection.h"
#include "../include/dsp.h"

float note_freq[NUM_NOTES];
static float note_score[NUM_NOTES];
int note_radius[NUM_NOTES];

float note_norm_factor[NUM_NOTES];

void compute_note_radius(int i, float f) {
	float semitone = f * 0.05946f;
	float half_width = semitone * 0.5f;

	float bins = half_width / ((float)SAMPLE_RATE / FFT_SIZE);

	int r = (int)(bins + 0.5f);
	if (r < 1)
		r = 1;
	if (r > MAX_BIN_RADIUS)
		r = MAX_BIN_RADIUS; // optional safety cap

	note_radius[i] = r;
}

void init_notes() {
	for (int i = 0; i < NUM_NOTES; i++) {
		int midi = i + 21;
		note_freq[i] = 440.0f * powf(2.0f, (midi - 69) / 12.0f);

		compute_note_radius(i, note_freq[i]);
	}
}

float *compute_note_scores(float *mag) {
	for (int n = 0; n < NUM_NOTES; n++) {
		float score = 0.0f;

		for (int h = 1; h <= MAX_HARMONICS; h++) {
			float f = note_freq[n] * h;

			if (f > SAMPLE_RATE / 2.0f)
				break;

			float bin_f = f * FFT_SIZE / SAMPLE_RATE;

			int bin_radius = note_radius[n];
			float sigma = SIGMA * bin_radius; // good default
			for (int k = -bin_radius; k <= bin_radius; k++) {
				int idx = (int)bin_f + k;
				if (idx < 1 || idx >= NUM_BINS - 1)
					continue;

				// gaussian weighting
				float diff = k;
				float weight = expf(-(diff * diff) / (2.0f * SIGMA * SIGMA));

				float harmonic_weight = (h == 1) ? 1.0f : 0.6f / h;
				score += mag[idx] * weight * harmonic_weight;
			}
		}
		note_score[n] = score;
		// printf("%.1f ", note_score[n]);
	}
	// println("");
	return note_score;
}

void compute_note_scores_simple(float *mag) {
	for (int n = 0; n < NUM_NOTES; n++) {
		float score = 0.0f;

		for (int h = 1; h <= MAX_HARMONICS; h++) {
			float f = note_freq[n] * h;

			if (f > SAMPLE_RATE / 2.0f)
				break;

			int bin = (int)(f * FFT_SIZE / SAMPLE_RATE);

			float weight = 1.0f / h; // reduce harmonic influence

			score += mag[bin] * weight;
		}

		note_score[n] = score;
	}
}

//----------------------------------------calc dominant note----------------------------------------

float hps[NUM_BINS];
void compute_hps(float *mag) {
	// Copy original spectrum
	for (int i = MIN_BIN; i < MAX_BIN; i++)
		hps[i] = mag[i];

	// Multiply downsampled versions
	for (int h = 2; h <= MAX_HARMONICS; h++) {
		for (int i = MIN_BIN; i < MAX_BIN / h; i++)
			hps[i] *= mag[i * h];
	}
}

float interpolate_peak(float *buf, int i) {
	float left = buf[i - 1];
	float center = buf[i];
	float right = buf[i + 1];

	float denom = (left - 2 * center + right);
	if (fabsf(denom) < 1e-6f)
		return i;

	return i + 0.5f * (left - right) / denom;
}

void detect_dominant_note() {
	uint32_t idx;
	float max_val;

	compute_hps(magnitude_buffer);
	arm_max_f32(&hps[MIN_BIN], MAX_BIN - MIN_BIN, &max_val, &idx);

	idx += MIN_BIN;

	float refined = interpolate_peak(hps, idx);

	float freq = refined * SAMPLE_RATE / FFT_SIZE;
}

uint16_t band_start[NUM_BANDS];
uint16_t band_end[NUM_BANDS];
float freq_bands[NUM_BANDS]; // usable bins

void init_log_bands() {
	for (int i = 0; i < NUM_BANDS; i++) {
		float frac1 = (float)i / NUM_BANDS;
		float frac2 = (float)(i + 1) / NUM_BANDS;

		float f1 = FREQ_MIN * powf(FREQ_MAX / FREQ_MIN, frac1);
		float f2 = FREQ_MIN * powf(FREQ_MAX / FREQ_MIN, frac2);

		band_start[i] = (uint16_t)(f1 * FFT_SIZE / SAMPLE_RATE);
		band_end[i] = (uint16_t)(f2 * FFT_SIZE / SAMPLE_RATE);

		if (band_start[i] < 1)
			band_start[i] = 1;
		if (band_end[i] >= NUM_BINS)
			band_end[i] = NUM_BINS - 1;

		printf("%d: %d, %d\n", i, band_start[i], band_end[i]);
	}
}
void normalized_bands() {
	// for (int b = 0; b < 50; b++) {
	for (int b = 0; b < NUM_BANDS; b++) {
		float sum = 0.0f;
		int count = 0;

		for (int i = band_start[b]; i < band_end[b]; i++) {
			sum += magnitude_buffer[i] / (FFT_SIZE / 2.0);
			// sum += magnitude_buffer[i];
			count++;
		}

		float avg = (count > 0) ? sum / count : 0.0f;

		float db = 20.0f * log10f(avg + EPSILON);

		if (db < DB_MIN)
			db = DB_MIN;

		freq_bands[b] = (db - DB_MIN) / (-DB_MIN);
		// freq_bands[b] = avg;
		// printf("%4.2f ", freq_bands[b]);
	}
	// println("");
}
