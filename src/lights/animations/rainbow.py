import time
import colorsys

from src import settings
from src.lights.vstrip import VStrip

def hsv_to_rgb(h, s, v):
    r, g, b = colorsys.hsv_to_rgb(h, s, v)
    return int(r * 255), int(g * 255), int(b * 255)

def create(led_count, speed = 0.1, hue_distance = 0.03):
    strip = VStrip(led_count)
    while True:
        t = time.time()
        for pos in range(led_count):
            hue = ((pos * hue_distance) + t * speed) % 1.0
            r, g, b = hsv_to_rgb(hue, 1.0, 1)
            strip.setPixelColor(pos + settings.START_OFFSET, (r, g, b))

        yield strip
