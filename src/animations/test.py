import time

from src import settings
from src.strip import VStrip

def create(led_count: int):
    strip = VStrip(led_count)
    direction = 0.5
    val = 3
    step = 0
    while True:
        for pos in range(led_count):
            # val = 0
            # if step % 7 == 0: val = 255

            # if pos == 1 or pos == 3:
            #     strip.setPixelColor(pos, (val, val, 0))
            # else:
            #     strip.setPixelColor(pos, (val, val, val))

            val = ((pos+1) / led_count) * 255
            strip.setPixelColor(pos + settings.START_OFFSET, (val, val, val))

        # if val >= 255: direction *= -1
        # elif val <= 0: direction *= -1
        # val += direction

        step += 1

        yield strip
