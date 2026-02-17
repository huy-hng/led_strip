from src import settings
from src.lights.vstrip import VStrip

def create(led_count: int):
    strip = VStrip(led_count)
    direction = 1
    val = 3
    step = 0
    hold_frames = 200
    while True:
        for pos in range(led_count):
            # val = 0
            # if step % 7 == 0: val = 255


            val = ((pos+1) / led_count) * 255
            strip.setPixelColor(pos + settings.START_OFFSET, (val, val, val))


        # if val >= 255:
        #     direction *= -1
        #     for _ in range(hold_frames): yield strip
        # elif val <= 0:
        #     direction *= -1
        #     for _ in range(hold_frames): yield strip
        # val += direction
        # step += 1

        yield strip
