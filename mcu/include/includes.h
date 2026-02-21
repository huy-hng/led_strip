#pragma once
#include <cmath>
#include <cstdio>
#include <cstring>

#include <hardware/adc.h>
#include <hardware/dma.h>

#include <pico/time.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/platform/common.h>

#ifdef CYW43_WL_GPIO_LED_PIN
#include <pico/cyw43_arch.h>
#endif

#include "../config.h"
#include "../include/pins.h"
#include "../include/util.h"
