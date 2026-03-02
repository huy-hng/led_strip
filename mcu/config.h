#pragma once

#define FFT_SIZE 1024 // Must be power of two
// #define SAMPLE_RATE 10240

// #define FFT_SIZE 2048 // Must be power of two
#define SAMPLE_RATE 20480 // lands 440Hz exactly in bin 22 (half of bin 11)

// max sample rate is ~512kHz
// #define SAMPLE_RATE 45056 // lands 440Hz exactly in bin 10
// #define SAMPLE_RATE 40960 // lands 440Hz exactly in bin 11
// #define SAMPLE_RATE 28160 // lands 440Hz exactly in bin 16
// #define SAMPLE_RATE 22528 // lands 440Hz exactly in bin 20
// #define SAMPLE_RATE 20480 // lands 440Hz exactly in bin 22 (half of bin 11)
// #define SAMPLE_RATE 14080 // lands 440Hz exactly in bin 32 (half of bin 16)

#define FFT_HOP_SIZE (FFT_SIZE / 4)
#define LOW_SAMPLING_RATE_FACTOR 8
