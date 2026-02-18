#pragma once

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

// Pin definitions taken from:
//    https://datasheets.raspberrypi.org/pico/pico-datasheet.pdf

#define PICO_RP2350A 1

// LEDs
#define PIN_LED			(0u)

// Serial
#define PIN_SERIAL1_TX	(0u)
#define PIN_SERIAL1_RX	(1u)

#define PIN_SERIAL2_TX	(8u)
#define PIN_SERIAL2_RX	(9u)

// SPI
#define PIN_SPI0_RX		(16u)
#define PIN_SPI0_CS		(17u)
#define PIN_SPI0_SCK	(18u)
#define PIN_SPI0_TX		(19u)

#define PIN_SPI1_RX		(12u)
#define PIN_SPI1_CS		(13u)
#define PIN_SPI1_SCK	(14u)
#define PIN_SPI1_TX		(15u)

// Wire
#define PIN_WIRE0_SDA	(4u)
#define PIN_WIRE0_SCL	(5u)

#define PIN_WIRE1_SDA	(26u)
#define PIN_WIRE1_SCL	(27u)

#define SERIAL_HOWMANY	(3u)
#define SPI_HOWMANY		(2u)
#define WIRE_HOWMANY	(2u)

// ADC
#define PIN_ADC0		(26u)
#define PIN_ADC1		(27u)
#define PIN_ADC2		(28u)
#define PIN_ADC3		(29u)
