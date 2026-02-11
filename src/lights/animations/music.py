import time
import numpy as np

from src.util import Timer
from src.strip import VStrip

from src.fft.dsp import fft_pipeline
from src.audio_input import start_stream
from src import settings

# TODO: alpha_channel gets smoothed out too
def create_valid_strip_from_values(values, size):
    values = np.pad(values, (0, abs(len(values)-size)))
    values = np.reshape(values, (len(values), 1))
    values = np.repeat(values, 3, axis=1)
    alpha_channel = np.reshape(np.repeat(255, len(values)), (len(values), 1))
    values = np.append(values, alpha_channel, axis=1)
    return values


def temporal_smoothing(prev, current):
    return settings.AUDIO_SMOOTHING * prev + (1 - settings.AUDIO_SMOOTHING) * current

def instant_attack_smoothing(prev, current):
    return np.where(current > prev, current, prev * settings.AUDIO_SMOOTHING)


def create(led_count: int):
    strip = VStrip(led_count)

    smoothing_fn = instant_attack_smoothing
    prev_vals = np.zeros(len(settings.FREQUENCY_RANGE))

    def get_data(buffer_array: np.ndarray):
        nonlocal prev_vals
        freqs = fft_pipeline(buffer_array)

        freqs = np.flip(freqs)

        if smoothing_fn:
            smoothed = smoothing_fn(prev_vals, freqs)
        else:
            smoothed = freqs

        prev_vals = smoothed

        for i, val in enumerate(freqs):
            if val < settings.MIN_DITHER:
                val = settings.MIN_DITHER
            strip.setPixelColor(i, (val, val, val))

        return True

    start_stream(get_data)

    while True:
        time.sleep(0.00)
        yield strip
