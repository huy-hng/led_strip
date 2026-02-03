const int pin_clk = 2;   // EC11 A
const int pin_dt = 3;   // EC11 B
const int pin_sw = 4;  // EC11 button (optional)

volatile int8_t direction = 0;
volatile uint8_t position = 0;
volatile uint8_t last_state = 0;

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
	
	// if (cw && value < 255) value++;
	// if (ccw && value > 0)  value--;
	if (direction > 0) {
		position++;
		// Serial.print(" cw: ");
		// Serial.println(position/4.0);
	} else if (direction < 0) {
		position--;
		// Serial.print("ccw: ");
		// Serial.println(position/4.0);
	}

	last_state = state;
}

void setup() {
	pinMode(pin_clk, INPUT_PULLUP);
	pinMode(pin_dt, INPUT_PULLUP);
	pinMode(pin_sw, INPUT_PULLUP);  // optional button

	last_state = (digitalRead(pin_clk) << 1) | digitalRead(pin_dt);

	attachInterrupt(digitalPinToInterrupt(pin_clk), isr, CHANGE);
	attachInterrupt(digitalPinToInterrupt(pin_dt), isr, CHANGE);

	Serial.begin(115200);
	Serial.println("EC11 rotary encoder ready");
}

void loop() {
	static long lastPos = 0;
	static bool lastButton = HIGH;

	// --- Encoder position ---
	if (position != lastPos) {
		noInterrupts();
		long p = position;
		interrupts();

		Serial.print("Position: ");
		Serial.println(p);

		lastPos = p;
	}

	// --- Optional button handling ---
	bool button = digitalRead(pin_sw);

	if (button == LOW && lastButton == HIGH) {
		Serial.println("Button pressed");
	}

	lastButton = button;
}
