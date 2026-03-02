// dsp.cpp
#include <cstdio>
#include "../include/dsp.h"

// ---------------------------
// FFT buffers (aligned for CMSIS-DSP)
// ---------------------------
__attribute__((aligned(32))) arm_rfft_fast_instance_f32 fft_instance;
__attribute__((aligned(32))) float fft_input[FFT_SIZE];
__attribute__((aligned(32))) float fft_output[FFT_SIZE];
float magnitude_buffer[FFT_SIZE/2+1]; // usable bins

// ---------------------------
// Initialize CMSIS-DSP FFT
// ---------------------------
void dsp_init() {
	// Initialize FFT instance
	arm_status status = arm_rfft_fast_init_f32(&fft_instance, FFT_SIZE);

	// printf("fft_instance.fftLen = %d\n", fft_instance.fftLen);
	// printf("fft_instance.pTwiddle = %p\n", fft_instance.pTwiddle);
	if (status != ARM_MATH_SUCCESS) {
		while (1) {
			printf("ERRORRRRR");
		}
	}
}

// ---------------------------
// Run FFT on current fft_input
// ---------------------------
void run_fft() {
	// Execute real FFT

	arm_rfft_fast_f32(&fft_instance, fft_input, fft_output, 0);

	// magnitude_buffer size = FFT_SIZE/2 + 1 = 513
	magnitude_buffer[0] = fabsf(fft_output[0]); // DC
	for (int k = 1; k < FFT_SIZE/2; k++) {
		float real = fft_output[2*k];
		float imag = fft_output[2*k + 1];
		magnitude_buffer[k] = sqrtf(real*real + imag*imag);
	}
	magnitude_buffer[FFT_SIZE/2] = fabsf(fft_output[1]); // Nyquist
}

// ---------------------------
// Example usage
// ---------------------------
void test_sine() {
	// Fill input with a test sine wave
	for (int i = 0; i < FFT_SIZE; i++) {
		fft_input[i] = sinf(2.0f * M_PI * 21.0f * i / FFT_SIZE);
	}


	run_fft();
	for (int i = 0; i <= FFT_SIZE / 2; i++) {
		printf("bin %d: %.6f\n", i, magnitude_buffer[i]);
	}

}
