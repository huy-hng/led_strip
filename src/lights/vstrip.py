import numpy as np
import numpy.typing as npt
from src import settings

pixel_type = npt.NDArray[np.float64]

class VStrip:
    def __init__(self, led_count: int, start_led_index: int = 0):

        # self.start_led_index = start_led_index
        self.led_count = led_count

        self.strip = np.zeros((settings.LED_COUNT, 4))
        self.strip[:, 3] = 1.0

    def setPixelColor(self, i: int, color: list | tuple | np.ndarray):
        if len(color) == 3:
            self.strip[i] = (*color, 1.0)
            return

        if len(color) == 4:
            self.strip[i] = color
            return

        raise Exception('invalid color')

    def getPixelColor(self, i: int) -> pixel_type:
        return self.strip[i]

    def numPixels(self) -> int:
        return self.led_count

    def turn_off(self):
        self.strip = np.zeros((settings.LED_COUNT, 4))

    def set_all_pixels(self, r, g, b):
        ...
