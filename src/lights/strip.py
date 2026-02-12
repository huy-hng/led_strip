from numba import njit, prange
import numpy as np
import numpy.typing as npt

from src.lights.vstrip import VStrip, pixel_type
from src import settings
from src.util import Timer

rgbf = npt.NDArray[np.float64]

@njit(fastmath=True)
def brightness_adjust(value: float) -> float:
    if value > 255: value = 255
    if value < 0: value = 0
    # gamma correction
    value = pow(value / 255.0, settings.GAMMA) * 255
    # set to global brightness
    return value * (settings.SIM_BRIGHTNESS/255)

@njit(fastmath=True)
def adjust_pixel(pixel: list[float], pos, dither_accum: rgbf):
    def dither(channel: int, value: float):
        nonlocal dither_accum

        if value < settings.MIN_DITHER: value = settings.MIN_DITHER
        value = value + dither_accum[pos][channel]
        remainder = value % 1
        dither_accum[pos][channel] = remainder
        return int(value)

    r = pixel[0]
    g = pixel[1]
    b = pixel[2]

    if r != 0:
        r = brightness_adjust(r)
        r = dither(0, r)

    if g != 0:
        g = brightness_adjust(g)
        g = dither(1, g)

    if b != 0:
        b = brightness_adjust(b)
        b = dither(2, b)

    if pos == 1 or pos == 3: b = 0
    return round(r), round(g), round(b)


@Timer(None, settings.TIME_BETWEEN_PRINTS)
# @njit(parallel=True, fastmath=True, cache=True)
def get_pixels(pixels, dither_accum, output_pixels):
    for pos in prange(settings.LED_COUNT): # ty: ignore
        output_pixels[pos] = adjust_pixel(pixels[pos], pos, dither_accum)
    return output_pixels


dither_accum = np.zeros((settings.LED_COUNT, 3), dtype=np.float32)
output_pixels = np.zeros((settings.LED_COUNT, 3), dtype=np.uint32)

# @Timer(None, settings.TIME_BETWEEN_PRINTS)
def show(pixel_strip, strips: list[VStrip]):

    # pixels = get_pixels(strips[0].strip, dither_accum, output_pixels)
    # for i, pix in enumerate(pixels):
    #     if i == 1 or i == 3: pix[2] = 0
    #     pixel_strip.setPixelColorRGB(i, pix[0], pix[1], pix[2])

    for pos in range(settings.LED_COUNT):

        final_pixel: pixel_type = np.array([0., 0., 0.], dtype=np.float16)

        transparency = 1

        for strip in strips:
            pix = strip.getPixelColor(pos)

            # final_pixel += pix[:3] * pix[3]
            final_pixel = pix[:3]

            # cur_col = np.array(v.getPixelColor(pos))
            # col += cur_col[:3] * transparency
            # transparency *= cur_col[3]


        r, g, b = adjust_pixel(final_pixel, pos, dither_accum)
        if pos == 1 or pos == 3: b = 0
        pixel_strip.setPixelColorRGB(pos, r, g, b)

    pixel_strip.show()

def clear(pixel_strip):
    [pixel_strip.setPixelColor(i, 0) for i in range(pixel_strip.numPixels())]
    pixel_strip.show()

def change_pixel(pixel_strip, pos, r=None, g=None, b=None):
    color = pixel_strip.getPixelColorRGB(pos)
    if r is None: r = color.r
    if g is None: g = color.g
    if b is None: b = color.b
    pixel_strip.setPixelColorRGB(pos, r, g, b)

