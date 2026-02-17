#include <math.h>

#include "FastLED.h"
#include "gamma_correction.h"

#define NUM_LEDS		200
#define NUM_LEDS_USED	100	
#define LED_PIN			2

// #define BAUD_RATE		9600
#define BAUD_RATE		115200
#define SHOULD_OUTPUT	true

void set_brightness(uint8_t brightness);

// const uint8_t default_brightness = 96;
const uint8_t default_brightness = 60;
volatile uint8_t encoder_position = default_brightness;
// brightness 128 already gets pretty warm
// 96 gets acceptably warm
uint8_t max_brightness;
struct CRGB leds[NUM_LEDS];

const uint8_t *default_gamma_correction = gamma_correction_e;

// rotary encoder
const uint8_t pin_clk = 14;   // Generating interrupts using CLK signal
const uint8_t pin_dt = 12;    // Reading DT signal
const uint8_t pin_sw = 13;    // Reading Push Button switch

volatile int8_t direction = 0;
volatile uint8_t last_state = 0;


void setup() {
	if (SHOULD_OUTPUT) {
		Serial.begin(BAUD_RATE);
	}

	// analogWrite(LED_PIN, 255);
	// analogWriteFreq(40000);

	FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
	set_brightness(encoder_position);
	FastLED.clear();
	FastLED.show();


	// encoder
	// pinMode(pin_clk, INPUT_PULLUP);
	// pinMode(pin_dt, INPUT_PULLUP);
	// pinMode(pin_sw, INPUT_PULLUP); // optional button

	// last_state = (digitalRead(pin_clk) << 1) | digitalRead(pin_dt);

	// attachInterrupt(digitalPinToInterrupt(pin_clk), isr, CHANGE);
	// attachInterrupt(digitalPinToInterrupt(pin_dt), isr, CHANGE);

	Serial.println("Ready");
}

void loop() {
	static uint8_t hue;
	static uint8_t last_brightness;

	if (max_brightness != last_brightness) {
		Serial.print("Brightness: ");
		Serial.println(max_brightness);

		last_brightness = max_brightness;
	}

	if (button_pressed()) {
		Serial.println("Button pressed");
		encoder_position = default_brightness;
		set_brightness(encoder_position);
	}

	FastLED.clear();

	// for(int i = 2; i < 22; i++) {
	// 	leds[i].setHSV(16*(i-2), 255, 255);
	// }

	// for(int i = 80; i < 100; i++) {
	// 	leds[i].setHSV(-16*i, 255, 255);
	// }

	// fade();
	// leds[0].setRGB(1,1,1);
	// brightness_scale();
	// fill_solid(leds, NUM_LEDS_USED, CRGB(255, 255, 255));
	rainbow_wave(12, 16);

	FastLED.show();
}

uint8_t gamma_correct(uint8_t brightness) {
	return pgm_read_byte(&default_gamma_correction[brightness]);
}

void set_brightness(uint8_t brightness) {
	max_brightness = gamma_correct(brightness);
	// if (max_brightness < 2) max_brightness = 2;
	FastLED.setBrightness(max_brightness);
}

int8_t turn_direction(uint8_t transition) {
	// Valid quadrature transitions (bounce-tolerant)
	if (transition == 0b0001 || transition == 0b0111 || transition == 0b1110 || transition == 0b1000)
		return 1;
	else if (transition == 0b0010 || transition == 0b0100 || transition == 0b1101 || transition == 0b1011)
		return -1;
	return 0;
}

void isr() {
	uint8_t clk = digitalRead(pin_clk);
	uint8_t dt = digitalRead(pin_dt);
	uint8_t state = (clk << 1) | dt;

	int8_t direction = turn_direction((last_state << 2) | state);


	if (direction > 0 && encoder_position < 255) {
		encoder_position++;
		set_brightness(encoder_position);
	}
	else if (direction < 0 && encoder_position > 0) {
		encoder_position--;
		set_brightness(encoder_position);
	}

	last_state = state;
}

boolean button_pressed() {
	boolean pressed = false;
	static bool last_button = HIGH;

	bool button = digitalRead(pin_sw);

	if (button == LOW && last_button == HIGH)
		pressed = true;
	last_button = button;

	return pressed;
}

void fade() {
	static uint8_t curr_brightness = 1;
	static int8_t direction = 1;

	if (curr_brightness==1) direction = 1;
	else if (curr_brightness==255) direction = -1;
	curr_brightness += direction;

	for(int i = 0; i < NUM_LEDS_USED; i++) {
		uint8_t normalized = gamma_correct(curr_brightness);
		leds[i].setRGB(normalized, normalized, normalized);
	}
}

void brightness_scale() {
	for(int i = 0; i < NUM_LEDS_USED; i++) {
		uint8_t brightness = 0;
		if (i != 0) brightness = (i*(256.0/NUM_LEDS_USED)-1);
		uint8_t normalized = gamma_correct(brightness);
		leds[i].setRGB(normalized, normalized, normalized);
	}
}

void rainbow_wave(uint8_t thisSpeed, uint8_t deltaHue) {
	// uint8_t thisHue = beatsin8(thisSpeed,0,255); // A simple rainbow wave.
	uint8_t thisHue = beat8(thisSpeed, 255); // A simple rainbow march.
	fill_rainbow(leds, NUM_LEDS_USED, thisHue, deltaHue);
}

