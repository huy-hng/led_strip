import time
import math
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor

import numba
from numba import jit, njit, typed, types, int16
from numba.experimental import jitclass

import numpy as np
from rpi_ws281x import PixelStrip

from src import settings
from src.profiler import Timer

timer = Timer('strip')



spec = [
    ('start_led_index', numba.int16),
    ('led_count', numba.int16),
    ('strip', types.ListType(types.ListType(numba.int16))),
]

# @jitclass(spec)
# @jitclass
class VStrip:
    def __init__(self, led_count: int, start_led_index: int = 0):

        self.start_led_index = start_led_index
        self.led_count = led_count


        # self.strip = np.zeros((settings.LED_COUNT, 4))
        self.strip = [(0, 0, 0, 0) for _ in range(settings.LED_COUNT)]

    def setPixelColor(self, i: int, color):
        if len(color) == 3:
            self.strip[i] = (*color, 1)
            return

        if len(color) == 4:
            self.strip[i] = color
            return

        raise Exception('invalid color')

    def getPixelColor(self, i):
        return self.strip[i]

    def numPixels(self):
        return self.led_count

gamma_table: list[float] = [
    pow(i / (settings.GAMMA_RESOLUTION-1), settings.GAMMA) * 255
    for i in range(settings.GAMMA_RESOLUTION)
]

@njit
def gamma_correction(value: float) -> float:
    return pow(value / 255.0, settings.GAMMA) * 255


@njit
def brightness_adjust(value: float) -> float:
    # value = clamp8(value)
    return gamma_correction(value) * (settings.SIM_BRIGHTNESS/255)
    # return gamma_table[round((settings.GAMMA_RESOLUTION/256) * value)] * (settings.SIM_BRIGHTNESS/255)
    # return value * (settings.SIM_BRIGHTNESS/255)

def clamp8(x):
    # if x > 255: return 255
    # if x < 0: return 0
    return x


class StripManager:
    strips: list[VStrip]

    def __init__(self, pixel_strip: PixelStrip):

        # self.accum_r = np.zeros(2, dtype=np.float16)
        self.accum_r = np.zeros(settings.LED_COUNT)
        self.accum_g = np.zeros(settings.LED_COUNT)
        self.accum_b = np.zeros(settings.LED_COUNT)

        self.pixel_strip = pixel_strip

        # self.executor = ThreadPoolExecutor(max_workers=32)
        # self.executor = ProcessPoolExecutor(max_workers=4)

    # @njit
    def adjust_pixel(self, pos):
        pixel = self.strips[0].getPixelColor(pos)

        r = pixel[0]
        g = pixel[1]
        b = pixel[2]

        if r != 0:
            r = brightness_adjust(r)
            r_remainder, r = math.modf(r + self.accum_r[pos])
            self.accum_r[pos] = r_remainder

        if g != 0:
            g = brightness_adjust(g)
            g_remainder, g = math.modf(g + self.accum_g[pos])
            self.accum_g[pos] = g_remainder

        if b != 0:
            b = brightness_adjust(b)
            b_remainder, b = math.modf(b + self.accum_b[pos])
            self.accum_b[pos] = b_remainder


        if pos == 1 or pos == 3: b = 0
        return r, g, b

    @timer.dec
    def show(self, strips: list[VStrip]):

        self.strips = strips

        for pos in range(settings.LED_COUNT):
            r, g, b = self.adjust_pixel(pos)
            self.pixel_strip.setPixelColorRGB(pos, round(r), round(g), round(b))

        self.pixel_strip.show()


    # def refresh(self):
    #     ...

    def clear(self):
        [self.pixel_strip.setPixelColor(i, 0) for i in range(self.pixel_strip.numPixels())]
        self.pixel_strip.show()


    def change_pixel(self, pos, r=None, g=None, b=None):
        color = self.pixel_strip.getPixelColorRGB(pos)
        if r is None: r = color.r
        if g is None: g = color.g
        if b is None: b = color.b
        self.pixel_strip.setPixelColorRGB(pos, r, g, b)

