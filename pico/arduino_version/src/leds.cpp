#include <Arduino.h>

// #define FASTLED_RP2040_PIO
#define FASTLED_OVERCLOCK 1.5
#include <FastLED.h>

#include "leds.h"

#define LED_PIN     6
#define NUM_LEDS    100
#define BRIGHTNESS  2
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

uint16_t print_every = 40;
static uint16_t step = 0;

struct CRGB leds[NUM_LEDS];

void off() {
	for(int i = 0; i < NUM_LEDS; i++) {
		leds[i].setRGB(0, 0, 0);
	}
}

void flicker() {
	uint8_t brightness = 0;
	if (step % 10 == 0)
		brightness = 255;
	for(int i = 0; i < NUM_LEDS; i++) {
		// if (i != 0) brightness = (i*(256.0/NUM_LEDS)-1);
		// uint8_t normalized = gamma_correct(brightness);
		leds[i].setRGB(brightness, brightness, brightness);
	}
}

void rainbow_wave(uint8_t thisSpeed, uint8_t deltaHue) {
	// uint8_t thisHue = beatsin8(thisSpeed,0,255); // A simple rainbow wave.
	uint8_t thisHue = beat8(thisSpeed, 255); // A simple rainbow march.
	fill_rainbow(leds, NUM_LEDS, thisHue, deltaHue);
}

void print_time(char name[], unsigned long start_time) {
	if (step % print_every == 0) {
		Serial.print(name);
		Serial.print(": ");
		Serial.print((micros() - start_time)/1000.0);
		Serial.println("ms");
	}
}

void led_setup() {

	FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
	FastLED.setMaxRefreshRate(0);
    FastLED.setBrightness(BRIGHTNESS);

	FastLED.clear();
	FastLED.show();
}

void led_loop() {
	unsigned long start_time = micros();
	rainbow_wave(5, 7);
	print_time("animation", start_time);

	leds[1].b = 0;
	leds[3].b = 0;

	start_time = micros();
    FastLED.show();
	print_time("render", start_time);

	step++;
	// FastLED.delay(20);
}
