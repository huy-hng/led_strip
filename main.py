import time
import math
import threading

from rpi_ws281x import PixelStrip, ws, Color
import numpy as np

from src.strip import StripManager, VStrip
from src import settings
from src.animations import rainbow

def clear(strip):
    for i in range(strip.numPixels()):
        strip.setPixelColor(i, 0)
    strip.show()


def white(strip):
    for pos in range(strip.numPixels()):
        strip.setPixelColor(pos, Color(red=255, green=255, blue=255))


def main():
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
    # strip = VStrip(settings.LED_COUNT_USED, settings.START_OFFSET)
    strip_man = StripManager(pixel_strip)
    animations = [rainbow.create(settings.LED_COUNT_USED)]

    print('Press Ctrl-C to quit.')

    test = 0
    try:
        while True:
            # for pos in range(settings.LED_COUNT_USED):
            #     # hue = ((pos * 0.03) + t * 0.1) % 1.0
            #     # r, g, b = hsv_to_rgb(hue, 1.0, 1)
            #     # strip.setPixelColor(pos + settings.START_OFFSET, (r, g, b))

            #     val = ((pos+1) / settings.LED_COUNT_USED) * 255
            #     strip.setPixelColor(pos + settings.START_OFFSET, (val,val,val))


            start = time.perf_counter()

            vstrips = []
            for animation in animations:
                vstrips.append(next(animation))

            strip_man.show(vstrips)

            # print(f'{time.perf_counter()-start:.4f}')

            # time.sleep(2 / 1000.0)


    except KeyboardInterrupt:
        clear(pixel_strip)
        return

    input('Press any key to continue.')


if __name__ == '__main__':
    main()
