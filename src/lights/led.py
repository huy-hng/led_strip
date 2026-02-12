import atexit

from src.lights.ws2812_api import PixelStrip

from src import settings
from src.util import Timer
from src.lights import strip as strip_man
from src.lights.animations import music, rainbow, test

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
    # test.create(settings.LED_COUNT_USED)
    # rainbow.create(settings.LED_COUNT_USED, 0.1, 0.03),
    music.create(settings.LED_COUNT_USED),
]

step = 0
@Timer('minimal loop', settings.TIME_BETWEEN_PRINTS)
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


@Timer('led loop', settings.TIME_BETWEEN_PRINTS)
def normal():
    vstrips = []
    for animation in animations:
        try:
            vstrips.append(next(animation))
        except StopIteration:
            continue
    strip_man.show(pixel_strip, vstrips)

@atexit.register
def exit_handler() -> None:
    strip_man.clear(pixel_strip)

def run():
    global step
    print('Press Ctrl-C to quit.')

    while True:
        normal()
        step += 1
        # if step == 500: break


if __name__ == '__main__':
    run()
