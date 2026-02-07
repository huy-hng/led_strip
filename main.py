import cProfile
import pstats

import time
import atexit
import traceback

import numpy as np
from numba.typed import List

from rpi_ws281x import PixelStrip, ws, Color

from src.strip import StripManager, VStrip
from src import settings
from src.animations import rainbow, test
from src import profiler

timer = profiler.Timer('main')
timer.enable = False

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
strip_man = StripManager(pixel_strip)
animations = [
    test.create(settings.LED_COUNT_USED)
    # rainbow.create(settings.LED_COUNT_USED, 0.1, 0.03),
]

step = 0
def minimal():
    global step
    for pos in range(settings.LED_COUNT):
        # val = round(((pos+1) / settings.LED_COUNT) * settings.SIM_BRIGHTNESS)

        val = 0
        if step % 7 == 0: val = 1

        if pos == 1 or pos == 3: 
            pixel_strip.setPixelColorRGB(pos, val, val, 0)
        else:
            pixel_strip.setPixelColorRGB(pos, val, val, val)

    pixel_strip.show()
    time.sleep(2/1000)

def normal():
    vstrips = []
    for animation in animations:
        vstrips.append(next(animation))
    strip_man.show(vstrips)

def profiling_stats(pr, name):
    stats = pstats.Stats(pr)
    stats.sort_stats(pstats.SortKey.TIME)
    stats.dump_stats(filename=f'/home/pi/repositories/led_strip/{name}.prof')
    # stats.print_stats()


@atexit.register
def exit_handler() -> None:
    strip_man.clear()

def main():
    global step
    print('Press Ctrl-C to quit.')

    # start = time.perf_counter()
    while True:
        timer.start()

        # if step >= 500: return

        normal()
        # minimal()
        time.sleep(2/1000)

        timer.end()
        step += 1


if __name__ == '__main__':

    try:
        main()
    # except KeyboardInterrupt:
    #     strip_man.clear()

    except Exception as e:
        print()
        traceback.print_exc()
        print()
        input('Press any key to continue.')



    # with cProfile.Profile() as pr:
    #     pr.disable()
    #     main()

    # profiling_stats(pr, 'normal')
    # strip_man.clear()
