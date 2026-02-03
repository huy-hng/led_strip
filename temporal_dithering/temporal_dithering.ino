#include <FastLED.h>

#define LED_PIN     5
#define NUM_LEDS    60
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define BAUD_RATE	115200

CRGB leds[NUM_LEDS];

// Fractional brightness accumulator (8.8 fixed point)
uint16_t brightnessAccum[NUM_LEDS];

uint8_t baseBrightness = 10;   // Low brightness where dithering helps
uint8_t noiseAmount    = 1;    // 0–8 is usually subtle and safe

void setup() {
	Serial.begin(BAUD_RATE);
	FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
	FastLED.clear();
	FastLED.show();

	// Initialize accumulators
	for (int i = 0; i < NUM_LEDS; i++) {
		brightnessAccum[i] = random(0, 256);
	}

	randomSeed(analogRead(A0));
}

void loop() {
	for (int i = 0; i < NUM_LEDS; i++) {

		// Example color source (could be anything: gradients, audio, etc.)
		// CRGB baseColor = CHSV((i * 5 + millis() / 10) & 0xFF, 255, 255);
		uint8_t br = i;
		CRGB baseColor = CRGB(br, br, br);

		// --- TEMPORAL DITHERING ---

		// Convert brightness to fixed-point (8.8)
		uint16_t target = (uint16_t)baseBrightness << 8;
		
		// Add subtle random noise to break up static patterns
		int8_t noise = random(-noiseAmount, noiseAmount + 1);
		target += (int16_t)noise << 8;

		// Accumulate fractional brightness
		brightnessAccum[i] += target;

		// Extract the integer brightness part
		uint8_t ditheredBrightness = brightnessAccum[i] >> 8;


		// Keep only the fractional remainder
		brightnessAccum[i] &= 0x00FF;

		// Apply brightness to color
		CRGB c = baseColor;

		c.r = scale8_video(c.r, ditheredBrightness);
		c.g = scale8_video(c.g, ditheredBrightness);
		c.b = scale8_video(c.b, ditheredBrightness);

		// c.nscale8_video(ditheredBrightness);
		leds[i] = c;


		// Serial.println("Ready");

		FastLED.delay(16); // ~60 FPS
	}

	FastLED.show();

	// Control frame rate (important for temporal dithering quality)
	// FastLED.delay(16); // ~60 FPS
}

