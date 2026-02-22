#include "../include/includes.h"

void printr(const char *format, ...) {
	char new_text[200];

	va_list args;
	va_start(args, format);

	vsnprintf(new_text, sizeof(new_text), format, args);
	printf("%s                                     \r", new_text);

	va_end(args);
}
void println(const char *format, ...) {
	char new_text[200];

	va_list args;
	va_start(args, format);

	vsnprintf(new_text, sizeof(new_text), format, args);
	printf("%s\n", new_text);

	va_end(args);
}

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

void generate_sine(float *arr) {
	for (int i = 0; i < FFT_SIZE; i++)
		arr[i] = sinf(2.0f * M_PI * 21.0f * i / FFT_SIZE);
}

float freq_to_bin_num(float freq) { return (freq / SAMPLE_RATE) * FFT_SIZE; }
float bin_num_to_freq(float bin) { return (bin / FFT_SIZE) * SAMPLE_RATE; }

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
