#include <dsp/statistics_functions.h>
#include "../include/includes.h"

#include "../include/note_detection.h"
#include "../include/dsp.h"

float note_freq[NUM_NOTES];
int note_radius[NUM_NOTES];

void compute_note_radius(int i, float f);

void init_notes() {
	for (int i = 0; i < NUM_NOTES; i++) {
		int midi = i + 21;
		note_freq[i] = 440.0f * powf(2.0f, (midi - 69) / 12.0f);

		compute_note_radius(i, note_freq[i]);
	}
}

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

void compute_note_scores(float *mag, float *note_score, uint16_t start_note, uint16_t end_note, uint32_t sample_rate) {
	int pos = 0;
	for (int n = start_note; n < end_note; n++) {
		float score = 0.0f;

		for (int h = 1; h <= MAX_HARMONICS; h++) {
			float f = note_freq[n] * h;

			if (f > sample_rate / 2.0f)
				break;

			float bin_f = f * FFT_SIZE / sample_rate;

			int bin_radius = note_radius[n];
			float sigma = SIGMA * bin_radius;
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
		// note_score[pos++] = score;
		note_score[n] = score;
		// printf("%.1f ", note_score[n]);
	}
	// println("");
}
