import numpy as np

from src.util import Timer
from src.strip import VStrip

from src.fft.fft import fft_pipeline
from src.fft.notes import get_frequency_list
from src.audio_input import start_stream

alpha = 0.9

# TODO: alpha_channel gets smoothed out too
def create_valid_strip_from_values(values, size):
    values = np.pad(values, (0, abs(len(values)-size)))
    values = np.reshape(values, (len(values), 1))
    values = np.repeat(values, 3, axis=1)
    alpha_channel = np.reshape(np.repeat(255, len(values)), (len(values), 1))
    values = np.append(values, alpha_channel, axis=1)
    return values


def temporal_smoothing(prev, current):
    return alpha * prev + (1 - alpha) * current

def instant_attack_smoothing(prev, current):
    return np.where(current > prev, current, prev * alpha)


def create(led_count: int):
    strip = VStrip(led_count)

    smoothing_fn = temporal_smoothing
    prev_vals = np.zeros(len(get_frequency_list()))

    get_freqs = start_stream(fft_pipeline)

    while True:
    # for freqs in start_stream(fft_pipeline):
        with Timer('fft', 60):
            freqs = next(get_freqs)

        freqs = np.flip(freqs)

        if smoothing_fn:
            smoothed = smoothing_fn(prev_vals, freqs)
        else:
            smoothed = freqs

        prev_vals = smoothed

        for i, val in enumerate(freqs):
            strip.setPixelColor(i, (val, val, val))

        yield strip


    strip.turn_off()
    yield strip
