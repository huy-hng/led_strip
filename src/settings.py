import math
from rpi_ws281x import ws

SIM_BRIGHTNESS	= 5
MIN_DITHER      = 0.15
# GAMMA			= math.e
GAMMA			= 2.2

START_OFFSET	= 0
LED_COUNT_USED	= 100		# Number of LED pixels.
LED_COUNT		= 100		# Number of LED pixels.
LED_PIN			= 18		# GPIO pin connected to the pixels (must support PWM!).
LED_FREQ_HZ		= 10*100000	# LED signal frequency in hertz (usually 800khz)
LED_DMA			= 10		# DMA channel to use for generating signal (try 10)
LED_BRIGHTNESS	= 255		# Set to 0 for darkest and 255 for brightest
LED_INVERT		= False		# True to invert the signal (when using NPN transistor level shift)
LED_CHANNEL		= 0			# set to '1' for GPIOs 13, 19, 41, 45 or 53

LED_STRIP		= ws.WS2812_STRIP
# LED_STRIP		= ws.WS2811_STRIP_GRB
