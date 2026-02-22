#pragma once

#define FFT_SIZE 1024 // Must be power of two
#define FFT_WINDOW_SIZE FFT_SIZE
#define FFT_HOP_SIZE (FFT_WINDOW_SIZE / 4)

// max sample rate is ~512kHz
// #define SAMPLE_RATE 45056 // lands 440Hz exactly in bin 10
// #define SAMPLE_RATE 40960 // lands 440Hz exactly in bin 11
#define SAMPLE_RATE 28160 // lands 440Hz exactly in bin 16
// #define SAMPLE_RATE 22528 // lands 440Hz exactly in bin 20
// #define SAMPLE_RATE 20480 // lands 440Hz exactly in bin 22 (half of bin 11)
// #define SAMPLE_RATE 14080 // lands 440Hz exactly in bin 32 (half of bin 16)
