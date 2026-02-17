import time
import colorsys
from rpi_ws281x import PixelStrip, Color

LED_COUNT = 100
FPS = 60

strip = PixelStrip(LED_COUNT, 18)
strip.begin()

dither_phase = 0
def scale8_video_dither(value, scale, dither_phase):
    """
    1-to-1 FastLED implementation:
    - value, scale: 0–255
    - dither_phase: 0–255
    """

    j = value * scale

    # FastLED video bias
    if value != 0 and scale != 0:
        j += 1

    # FastLED temporal dithering
    j += dither_phase

    return (j >> 8) & 0xFF

GAMMA = 2.2
gamma_table = [
    int(pow(i / 255.0, GAMMA) * 255 + 0.5)
    for i in range(256)
]

def clear(strip):
    for i in range(strip.numPixels()):
        strip.setPixelColor(i, 0)
    strip.show()

try:
    while True:
        t = time.time()

        for i in range(LED_COUNT):
            # Source color
            h = ((i * 0.03) + t * 0.05) % 1.0
            r, g, b = colorsys.hsv_to_rgb(h, 1.0, 0.1)

            r = int(r * 255)
            g = int(g * 255)
            b = int(b * 255)

            # Gamma (FastLED typical)
            r = gamma_table[r]
            g = gamma_table[g]
            b = gamma_table[b]

            # FastLED scaling + dithering (scale = 255)
            r = scale8_video_dither(r, 255, dither_phase)
            g = scale8_video_dither(g, 255, dither_phase)
            b = scale8_video_dither(b, 255, dither_phase)


            if i == 1 or i == 3: b = 0
            strip.setPixelColor(i, Color(r, g, b))

        strip.show()

        # EXACT FastLED behavior
        dither_phase = (dither_phase + 1) & 0xFF

        time.sleep(1.0 / FPS)

except KeyboardInterrupt:
    clear(strip)
    strip.show()
