#pragma once
#include "../include/includes.h"

uint64_t micros();
uint64_t millis();

void blink(uint32_t duration);

void println(const char *args, ...) _ATTRIBUTE((__format__(__printf__, 1, 2)));
void printr(const char *args, ...) _ATTRIBUTE((__format__(__printf__, 1, 2)));

void create_vertical_bar(float val);
void print_volume(uint16_t vol);

template <typename T>
T clamp(T min, T val, T max) {
	val = val > max ? max : val;
	val = val < min ? min : val;
	return val;
}

template <typename T> T mean(int amount, T *arr) {
	float avg = 0;
	for (int i = 0; i < amount; i++) {
		avg += arr[i];
	}
	return avg / amount;
}

template <typename T> float positive_mean(int amount, T *arr) {
	float avg = 0;
	for (int i = 0; i < amount; i++) {
		avg += abs(arr[i]);
	}
	return avg / amount;
}
