#include <pico/cyw43_arch.h>
#include "../include/pins.h"
#include "../include/util.h"

uint64_t micros() {
	return time_us_64();
	// return to_ms_since_boot(get_absolute_time());
}

uint64_t millis() {
	return time_us_64() / 1000;
	// return to_ms_since_boot(get_absolute_time()); // actually uint32_t
}

void blink(uint32_t duration) {
	gpio_init(CYW43_WL_GPIO_LED_PIN);
	// gpio_set_dir(PIN_LED, GPIO_OUT);
	// gpio_put(PIN_LED, false);

	cyw43_arch_gpio_put(PIN_LED, true);
	sleep_ms(duration);
	cyw43_arch_gpio_put(PIN_LED, false);
	sleep_ms(duration);
}

int mean(int amount, volatile uint16_t *arr) {
	float avg = 0;
	for (int i = 0; i < amount; i++) {
		avg += arr[i];
	}
	return avg / amount;
}




// repeating timer
// struct repeating_timer timer;
// add_repeating_timer_ms(1000, repeating_timer_callback, NULL, &timer);

// int * avg() {
// 	for (uint16_t j=0 ; j<65536 ; j+=1024) {
// 		for(uint16_t k=j ; k<j+1024 ; ++k)
// 			result[k] = arr[0][k];

// 		// Summation of the current block
// 		for(uint16_t i=1 ; i<7 ; ++i)
// 			for(uint16_t k=j ; k<j+1024 ; ++k)
// 				result[k] += arr[i][k];

// 		for(uint16_t k=j ; k<j+1024 ; ++k)
// 			result[k] /= 7;
// 	}
// }
