#pragma once

void init_dsp();
void fft_loop();
void fft_clean();


// #include "arm_math.h"
// #include <math.h>
// #include <stdint.h>

// #define FFT_SIZE 1024

// extern arm_rfft_fast_instance_f32 fft_instance;
// extern float fft_input[FFT_SIZE];
// extern float fft_output[FFT_SIZE];

// void dsp_init();
// void run_fft();
// void test_sine();
