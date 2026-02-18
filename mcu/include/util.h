#pragma once

#include "pico/stdlib.h"

uint64_t micros();
uint64_t millis();

void blink(uint32_t duration);

int mean(int amount, volatile uint16_t *arr);
void print_volume(uint16_t vol);
