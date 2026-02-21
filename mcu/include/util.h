#pragma once
#include "../include/includes.h"

uint64_t micros();
uint64_t millis();

void blink(uint32_t duration);

void println(const char *args, ...) _ATTRIBUTE((__format__(__printf__, 1, 2)));
void printr(const char *args, ...) _ATTRIBUTE((__format__(__printf__, 1, 2)));

int mean(int amount, uint16_t *arr);
void print_volume(uint16_t vol);
