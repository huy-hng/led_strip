#define LED_PIN	5
#define ANALOG_PIN	A0


int val = 0;         // variable to store the read value

void setup() {
	Serial.begin(115200);
	pinMode(LED_PIN, OUTPUT);  // sets the pin as output
}

void loop() {
	val = analogRead(ANALOG_PIN);  // read the input pin
	// Serial.println(val);
	analogWrite(LED_PIN, val / 4); // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
	// analogWrite(LED_PIN, 20); // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
	delay(50);
}
