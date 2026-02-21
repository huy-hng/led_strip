#pragma once

#define FFT_WINDOW_SIZE	(1024)	// Must be power of two
#define FFT_HOP_SIZE	(FFT_WINDOW_SIZE / 4)

// max sample rate is ~512kHz
#define SAMPLE_RATE (48000)
