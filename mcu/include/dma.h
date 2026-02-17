#pragma once
#include "pico/stdlib.h"

#define FRAME_SIZE 64

volatile bool frame_ready;
uint8_t *dma_buf; // NOLINT

void init_dma();
