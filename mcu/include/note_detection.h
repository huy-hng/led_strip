#pragma once
#include "../include/includes.h"

#define MAX_HARMONICS 1
#define MAX_BIN_RADIUS 8

#define SIGMA 0.4f

#define FREQ_MIN 27.5f
#define FREQ_MAX 4200.0f

#define NUM_NOTES 100
#define NUM_NOTES_LOW 48
// #define NUM_NOTES_LOW 0

void init_notes();
void detect_dominant_note();
void compute_note_scores(float *mag, float *note_score, uint16_t start_note, uint16_t end_note, uint32_t sample_rate);

extern float frequencies[];
