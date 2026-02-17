// #include "src/leds.h"
#include "src/communication.h"

#define BAUD_RATE		115200
#define SHOULD_OUTPUT	true

char *format_text(char *fmt, ...) {
	static char new_text[100];

	va_list args;
	va_start(args, fmt);

	vsprintf(new_text, fmt, args);
	return new_text;
}

void setup() {
	if (SHOULD_OUTPUT) Serial.begin(BAUD_RATE);

	communication_setup();
}


void loop() {
	communication_loop();
	// led_loop();
}
