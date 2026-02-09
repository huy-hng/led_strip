import bisect
import time
from itertools import zip_longest
import sys
import math

import numpy as np
from matplotlib import pyplot as plt
from scipy.io import wavfile
from scipy.fft import rfft, rfftfreq
from scipy.signal import find_peaks

from src import settings
from src.fft import notes, plotter
from src.profiler import Timer
timer = Timer('fft', 1)

# np.set_printoptions(threshold=sys.maxsize)

# ax.set_xlim(left=0, right=3520)
# ax.set_ylim(bottom=0, top=7.2e6)

# 'f#5,      a#5, c#6, h5, d#6, f#6, c#7'
# 'f#4, a#4,           h4,           f#5, a#5'
# [740,      932, 1109,988,1245,1480,2217]
# [370, 466,           494,          740, 932]

def map_to_closest(arr1, arr2):
    indices = []

    for x in arr1:
        i = bisect.bisect_left(arr2, x)

        if i == 0:
            closest_i = 0
        elif i == len(arr2):
            closest_i = len(arr2) - 1
        else:
            before = arr2[i - 1]
            after = arr2[i]
            closest_i = i - 1 if abs(x - before) <= abs(x - after) else i

        indices.append((closest_i, arr2[closest_i]))
        # indices.append(closest_i)

    return indices

def chunker(iterable, n, fillvalue=None):
    args = [iter(iterable)] * n
    return zip_longest(*args, fillvalue=fillvalue)

def filter_by_notes(yf, xf):
    frequencies = map_to_closest(notes.piano_frequencies, xf)

    transformed = np.zeros(len(frequencies))
    region_factor = 30
    for i, f in enumerate(frequencies):
        start = -int(f[1]/region_factor) + f[0]
        end = int(f[1]/region_factor) + f[0]
        transformed[i] = sum(yf[start:end]) # type: ignore


    transformed *= 255 / transformed.max()

    plot = plotter.simple_plot([freq for _, freq in frequencies], transformed)
    plotter.save_plot(plot, 'fft')

def detect_peaks(xf, yf):
    mag = np.abs(yf)

    height = np.max(mag) * 0.1
    prominence = np.max(mag) * 0.05
    distance = 5

    peaks, props = find_peaks(
        mag,
        height=height, #threshold
        distance=distance, #min_bin_spacing
        prominence=prominence
    )

    peak_freqs = xf[peaks]
    peak_mags  = mag[peaks]

    plot = plotter.simple_plot(peak_freqs, peak_mags)
    plotter.save_plot(plot, 'peaks')

def filter_peaks(times, freqs, S_db):
    filtered = np.zeros_like(S_db)
    for i, _ in enumerate(times):
        yf = S_db[:, i]
        mag = np.abs(yf)

        height = np.max(mag) * 0.1
        prominence = np.max(mag) * 0.05
        distance = 5

        peaks, props = find_peaks(
            mag,
            height=height, #threshold
            distance=distance, #min_bin_spacing
            prominence=prominence
        )

        mags = np.zeros_like(mag)
        mags[peaks] = mag[peaks]
        filtered[:, i] = mags

    return times, freqs, filtered

def windowing(mono, sample_rate, window_size, hop_size):
    buffer = np.zeros(window_size)
    def update(new_samples):
        nonlocal buffer
        buffer = np.roll(buffer, -len(new_samples))
        buffer[-len(new_samples):] = new_samples
        yf = rfft(buffer)
        return yf

    num_windows = math.ceil(len(mono)/hop_size)

    xf = rfftfreq(window_size, 1 / sample_rate)
    transformed = np.zeros((num_windows, len(xf)))

    for i, chunk in enumerate(chunker(mono, int(hop_size), 0)):
        # timer.start()
        yf = update(chunk)
        # timer.end(f'fft duration iteration {i}')
        transformed[i] = np.abs(yf) # type: ignore

    transformed = np.swapaxes(transformed, 0, 1)

    # transformed = 20 * np.log10(np.abs(transformed) + 1e-12)

    times = [(hop_size / sample_rate) * i for i in range(num_windows)]

    # times, xf, transformed = filter_peaks(times, xf, transformed)
    return times, xf, transformed


def test_params(mono, sample_rate, win, hop):
    timer.start()
    data = windowing(mono, sample_rate, win, hop)
    timer.end(f'fft {win=} {hop=}')

    timer.start()
    plotter.spectrogram(*data, f'fft_win{win}_hop{hop}_spectrogram', fmax=2500)
    timer.end('saving')
    print()


def main():
    sample_rate, wav = wavfile.read('/home/pi/repositories/led_strip/assets/BotW - Item.wav')
    mono = wav.mean(axis=1)
    # sample_duration = len(mono) / sample_rate

    params = [
        # (2048, 512),
    ]
    for param in params:
        test_params(mono, sample_rate, param[0], param[1])

    # image *= 255.0/image.max()
    # xf, yf = fft(mono, sample_rate)
    # detect_peaks(xf, yf)
