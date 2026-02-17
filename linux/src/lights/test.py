import atexit
import time
import colorsys
from concurrent.futures import InterpreterPoolExecutor

from src.lights.ws2812_api import PixelStrip, rgbw_to_24bit

from src import settings
from src.util import Timer
from src.lights import strip as strip_man

pixel_strip = PixelStrip(
    settings.LED_COUNT,
    settings.LED_PIN,
    settings.LED_FREQ_HZ,
    settings.LED_DMA,
    settings.LED_INVERT,
    settings.LED_BRIGHTNESS,
    settings.LED_CHANNEL,
    settings.LED_STRIP
)
pixel_strip.begin()

step = 0

def test():
    global step

    val = 0
    if step % 4 == 0: val = 1

    # pixels = [[val, val, val] for _ in range(settings.LED_COUNT)]
    # pixels[1][2] = 0
    # pixels[3][2] = 0

    with Timer('settings pixels', 1):
        pixel_strip[:] = rgbw_to_24bit(val, val, val)


    step += 1

hue_distance = 0.1
speed = 1
def rainbow():
    global step
    t = time.time()

    val = 0
    if step % 4 == 0: val = 1
    step += 1

    with Timer('settings pixels', 1):
        for pos in range(settings.LED_COUNT):
            hue = ((pos * hue_distance) + t * speed) % 1.0
            r, g, b = colorsys.hsv_to_rgb(hue, 1.0, 1)
            r, g, b = int(r * val * settings.SIM_BRIGHTNESS), int(g * val * settings.SIM_BRIGHTNESS), int(b * val * settings.SIM_BRIGHTNESS)
            if pos == 1 or pos == 3: b = 0

            pixel_strip.setPixelColorRGB(pos, r, g, b)


def minimal():
    global step

    val = 0
    if step % 40 == 0: val = 1
    color = rgbw_to_24bit(val, val, val)

    with Timer('set pixels', 1):
        for pos in range(settings.LED_COUNT):
            new_color = color
            if pos == 1 or pos == 3:
                new_color = rgbw_to_24bit(val, val, 0)

            pixel_strip.setPixelColor(pos, new_color)

    step += 1

@atexit.register
def exit_handler() -> None:
    strip_man.clear(pixel_strip)

def run():

    while True:
        rainbow()
        # minimal()
        # test()
        # start: float = time.perf_counter()
        # print(f'{(time.perf_counter() - start)*1000:.2f}')

        with Timer('render', 1):
            pixel_strip.show()


if __name__ == '__main__':
    run()
