import numpy as np

from src.strip import VStrip
from src.fft.fft_testing import get_mock_audio
from src.fft.notes import get_frequency_list

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

    prev_vals = np.zeros(len(get_frequency_list()))

    def loop(smoothing_fn=None):
        nonlocal prev_vals
        freqs = get_mock_audio()
        for values in freqs:
            if values is None: continue
            values = np.flip(values)

            if smoothing_fn:
                smoothed = smoothing_fn(prev_vals, values)
            else:
                smoothed = values

            prev_vals = smoothed

            # for v, s in zip(values, smoothed):
            #     print(v, round(s, 2))
            # input('Press any key to continue.')

            for i, val in enumerate(values):
                strip.setPixelColor(i, (val, val, val))

            yield strip

    # while True:

    for strip in loop():
        yield strip

    for strip in loop(temporal_smoothing):
        yield strip

    for strip in loop(instant_attack_smoothing):
        yield strip

    strip.turn_off()
    yield strip
