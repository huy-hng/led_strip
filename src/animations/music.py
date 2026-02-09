from src.strip import VStrip
from src.fft.fft import main

def create(led_count: int):
    strip = VStrip(led_count)
    transformed = main()
    while True:
        for values in transformed:
            for i, val in enumerate(values):
                strip.setPixelColor(i, (val, val, val))

            yield strip
