#pragma once
#include "../include/includes.h"

void init_visualizer();
void print_volume(uint16_t vol);
void visualize_fft();
void visualize_fft_horizontal();
char *create_horizontal_bar(float val);
void print_spectrogram(float *mags);
void print_notes(float *notes, uint16_t num_notes);
void print_bands();
