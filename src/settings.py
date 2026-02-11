from rpi_ws281x import ws
import subprocess

EPSILON = 1e-12

result = subprocess.run(['hostname'], capture_output=True)
hostname = result.stdout.decode('utf-8').rstrip()

if hostname == 'nixos':
    project_path = '/home/huy/mnt/rpi_zero/home/pi/repositories/led_strip'
else:
    project_path = '/home/pi/repositories/led_strip'

#-------------------------------------------LED Settings--------------------------------------------
SIM_BRIGHTNESS	= 1
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


#-------------------------------------------FFT Settings--------------------------------------------

FFT_WINDOW_SIZE = 1024
# FFT_HOP_SIZE = 256
FFT_HOP_SIZE = FFT_WINDOW_SIZE / 4
FFT_NOISE_GATE = 0.02
FFT_PEAK_DECAY = 0.995
FFT_STARTING_PEAK = 10e5

#--------------------------------------------Audio Input--------------------------------------------
INPUT_SAMPLE_RATE = 48000
# OUTPUT_SAMPLE_RATE = 24000
FPS = 60
