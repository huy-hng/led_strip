#pragma once

#define MAX_HARMONICS 1
#define MAX_BIN_RADIUS 8

#define SIGMA 0.4f

#define FREQ_MIN 27.5f
#define FREQ_MAX 4200.0f

#define NUM_NOTES 88

void init_notes();
void detect_dominant_note();
float *compute_note_scores(float *mag);

extern float frequencies[];
