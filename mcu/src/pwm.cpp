#include "hardware/pwm.h"
#include "../include/includes.h"

void function_gen() {
	uint pin = 16;
	gpio_set_function(pin, GPIO_FUNC_PWM);
	gpio_set_function(pin+1, GPIO_FUNC_PWM);
	uint slice_num = pwm_gpio_to_slice_num(pin);

	// Set period of 4 cycles (0 to 3 inclusive)
	pwm_set_wrap(slice_num, 3);
	// Set channel A output high for one cycle before dropping
	pwm_set_chan_level(slice_num, PWM_CHAN_A, 2);
	// Set initial B output high for three cycles before dropping
	pwm_set_chan_level(slice_num, PWM_CHAN_B, 1);
	// Set the PWM running
	pwm_set_enabled(slice_num, true);
}
