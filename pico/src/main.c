#include <wchar.h>
#include <stdio.h>
#include <pico/time.h>
#include "hardware/adc.h"
#include "pico/stdlib.h"

#include "pins.h"

// uint32_t bar[] = {0x258F, 0x258E, 0x258D, 0x258C, 0x258B, 0x258A, 0x2589, 0x2588};


// █▓▒░ transparency
// ▁▂▃▄▅▆▇██
// 123456789


char bar[9][4] = {" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"};


void print_volume(uint16_t vol) {
	int bar_volume = 4096 / 16;
	int full_bars = vol / bar_volume;

	
	char volbar[full_bars*4 + 5];
	for (int i=0; i<full_bars; i++) {
		// strcat(volbar, bar[8]);
		strcat(volbar, "0");
	}

	float remainder = ((float) vol / bar_volume) - full_bars;
	remainder = remainder < 0.1 ? 0 : remainder;
	char *remainder_bar = bar[(int) (remainder * 8)];
	// printf("%d, %.2f\n", (int)(remainder * 8), remainder);

	// strcat(volbar, remainder_bar);
	printf("%4d %s\n", vol, volbar);
	// printf("%4d %d %.1f\n", vol, full_bars, remainder);
}


void init() {
	cyw43_arch_init();

	// gpio_init(CYW43_WL_GPIO_LED_PIN);
	gpio_init(PIN_LED);
	gpio_set_dir(PIN_LED, GPIO_OUT);

    stdio_init_all(); // Initialize UART/USB stdio

	adc_init();
    adc_gpio_init(26);   // GPIO26 = ADC0
    adc_select_input(0);
	
}

void loop() {
	uint16_t result = adc_read();
	// float voltage = result * 3.3f / 4095.0f;
	// printf("ADC: \t %d \t Voltage: %.2f\n", result, voltage);
	print_volume(result);

	sleep_ms(20);
}

int main() {
	init();
    while (true)
		loop();
}

void blink() {
	// gpio_put(PIN_LED, false);
	cyw43_arch_gpio_put(PIN_LED, true);
	sleep_ms(1000);
	cyw43_arch_gpio_put(PIN_LED, false);
	sleep_ms(1000);
}
