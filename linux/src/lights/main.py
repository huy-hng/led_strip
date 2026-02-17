import atexit
import time

import numpy as np

from src.lights.ws2812_api import PixelStrip

from src import settings
from src.util import Timer, print_every_x_times
from src import global_variables as globals
from src.lights import strip as strip_man
from src.lights.animations import music, rainbow, test

global_step = 0

refresh_times = np.zeros(10)

def update_refresh_time(time_taken):
    refresh_times[:len(refresh_times)-1] = refresh_times[1:]
    refresh_times[-1] = time_taken

def get_refresh_time():
    return refresh_times.mean()

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
animations = [
    test.create(settings.LED_COUNT_USED)
    # rainbow.create(settings.LED_COUNT_USED, 0.1, 0.03),
    # music.create(settings.LED_COUNT_USED),
]

@atexit.register
def exit_handler() -> None:
    strip_man.clear(pixel_strip)


@Timer('Main Loop', settings.TIME_BETWEEN_PRINTS)
def loop():
    vstrips = []
    for animation in animations:
        try:
            # with Timer('get animation', settings.TIME_BETWEEN_PRINTS):
                vstrips.append(next(animation))
        except StopIteration:
            continue

    strip_man.show(pixel_strip, vstrips)

def run():
    print('Press Ctrl-C to quit.')
    while True:
        start_time = time.perf_counter()
        loop()

        update_refresh_time(time.perf_counter() - start_time)

        globals.step += 1


if __name__ == '__main__':
    run()
