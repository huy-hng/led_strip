import math
import random

from rpi_ws281x import PixelStrip

from src import settings

class VStrip:
    def __init__(self, led_count: int, start_led_index: int = 0):
        self.start_led_index = start_led_index
        self.led_count = led_count

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

def random_round(val, rand):
    if val - math.floor(val) >= rand:
        return math.ceil(val)
    return math.floor(val)


gamma_table = [pow(i / 255.0, settings.GAMMA) * 255 for i in range(256)]

def brightness_adjust(value):
    # return gamma_table[round(value)] * (settings.SIM_BRIGHTNESS/255)
    return value * (settings.SIM_BRIGHTNESS/255)

class StripManager:
    def __init__(self, pixel_strip: PixelStrip):

        # self.brightness_accum = [random.random() for _ in range(pixel_strip.numPixels())]
        led_count = pixel_strip.numPixels()

        # self.accum_r = [random.random() for _ in range(led_count)]
        # self.accum_g = [random.random() for _ in range(led_count)]
        # self.accum_b = [random.random() for _ in range(led_count)]

        self.accum_r = [0.0 for _ in range(led_count)]
        self.accum_g = [0.0 for _ in range(led_count)]
        self.accum_b = [0.0 for _ in range(led_count)]

        self.pixel_strip = pixel_strip
        self.step = 0

    def show(self, strips: list[VStrip]):
        rand = random.random()
        for pos in range(self.pixel_strip.numPixels()):
            pix = strips[0].getPixelColor(pos)

            r = pix[0] * (settings.SIM_BRIGHTNESS/255)
            g = pix[1] * (settings.SIM_BRIGHTNESS/255)
            b = pix[2] * (settings.SIM_BRIGHTNESS/255)

            r = brightness_adjust(pix[0])
            g = brightness_adjust(pix[1])
            b = brightness_adjust(pix[2])

            r_remainder, r = math.modf(r + self.accum_r[pos])
            g_remainder, g = math.modf(g + self.accum_g[pos])
            b_remainder, b = math.modf(b + self.accum_b[pos])

            self.accum_r[pos] = r_remainder
            self.accum_g[pos] = g_remainder
            self.accum_b[pos] = b_remainder

            # r = random_round(r, rand)
            # g = random_round(g, rand)
            # b = random_round(b, rand)

            if pos == 1 or pos == 3: b = 0
            self.pixel_strip.setPixelColorRGB(pos, round(r), round(g), round(b))

        self.pixel_strip.show()
        self.step += 1
        # if self.step % 50 == 0:
        #     print()

    def change_pixel(self, pos, r=None, g=None, b=None):
        color = self.pixel_strip.getPixelColorRGB(pos)
        if r is None: r = color.r
        if g is None: g = color.g
        if b is None: b = color.b
        self.pixel_strip.setPixelColorRGB(pos, r, g, b)

