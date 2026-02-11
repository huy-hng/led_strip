import sys
import math
import time
from itertools import zip_longest

import numpy as np
from scipy.io import wavfile
from scipy.fft import rfftfreq
from scipy.signal import find_peaks

from src import settings
from src.settings import EPSILON, path_to_assets
from src.fft import plotter, fft, notes
from src.util import Timer
from src.audio_input import start_stream

np.set_printoptions(threshold=sys.maxsize)

# ax.set_xlim(left=0, right=3520)
# ax.set_ylim(bottom=0, top=7.2e6)

# 'f#5,      a#5, c#6, h5, d#6, f#6, c#7'
# 'f#4, a#4,           h4,           f#5, a#5'
# [740,      932, 1109,988,1245,1480,2217]
# [370, 466,           494,          740, 932]

def get_audio():
    audio_file = path_to_assets + 'BotW - Item.wav'
    # audio_file = 'Frederic_Chopin_-_Nocturne_Eb_major_Opus_9,_number_2.wav'

    sample_rate, wav = wavfile.read(audio_file)
    # sample_duration = len(mono) / sample_rate
    print(sample_rate, wav.shape)

    audio = wav.mean(axis=1)

    return sample_rate, audio


def chunker(iterable, n, fillvalue=None):
    args = [iter(iterable)] * n
    return zip_longest(*args, fillvalue=fillvalue)

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


def looper(sample_rate, audio, wait=True):
    chunks = chunker(audio, int(settings.FFT_HOP_SIZE), 0)
    wait_time = settings.FFT_HOP_SIZE / sample_rate
    start_time = time.perf_counter()

    current_peak = settings.FFT_STARTING_PEAK

    while True:

        if wait and time.perf_counter() - start_time < wait_time:
            yield
            continue

        start_time = time.perf_counter()

        try:
            new_samples = next(chunks)
        except StopIteration:
            if not wait: return
            return np.zeros(settings.LED_COUNT)

        yf = fft.update(new_samples)

        # logarithmic compression
        # yf = np.log1p(yf)
        # # mags_db = 20*np.log10(yf + EPSILON)


        # bands = fft.map_fft_to_log_bands(
        #     yf,
        #     fs=sample_rate,
        #     N=settings.FFT_WINDOW_SIZE,
        #     band_centers=notes.piano_frequencies
        # )

        # bands = fft.filter_peaks(bands)

        # # peak tracking
        # current_peak = max(bands.max(), current_peak * settings.FFT_PEAK_DECAY)

        # # normalize
        # normalized = bands / (current_peak + EPSILON)

        # # noise gate
        # normalized[normalized < settings.FFT_NOISE_GATE] = 0

        # # map to 255
        # normalized = np.clip(normalized * 255, 0, 255).astype(np.uint8)
        # yield normalized
        yield yf



@Timer()
def banding(sample_rate, audio):
    num_windows = math.ceil(len(audio)/settings.FFT_HOP_SIZE)

    bands_history = np.zeros((num_windows, len(notes.piano_frequencies)))

    for i, bands in enumerate(looper(sample_rate, audio, wait=False)):
        bands_history[i] = bands

    bands_history = np.swapaxes(bands_history, 0, 1)

    times = [(settings.FFT_HOP_SIZE / sample_rate) * i for i in range(num_windows)]
    return times, notes.piano_frequencies, bands_history


@Timer()
def create_spectrogram(len_frequencies):
    seconds_to_record = 4
    samples_to_record = seconds_to_record * settings.INPUT_SAMPLE_RATE
    num_windows = math.ceil(samples_to_record/settings.FFT_HOP_SIZE)

    history = np.zeros((num_windows, len_frequencies))

    for i, yf in enumerate(start_stream(fft.fft_pipeline)):
    # for i, yf in enumerate(looper(settings.INPUT_SAMPLE_RATE, audio, wait=False)):
        history[i] = yf
        if i == num_windows: break

    history = np.swapaxes(history, 0, 1)

    times = [(settings.FFT_HOP_SIZE / settings.INPUT_SAMPLE_RATE) * i for i in range(num_windows)]
    return times, history


# sample_rate, audio = get_audio()
# def get_mock_audio():
#     return looper(sample_rate, audio)


def test():
    frequencies = notes.get_frequency_list()
    fmax = frequencies[-4]
    fmin = frequencies[12]

    xf = rfftfreq(settings.FFT_WINDOW_SIZE, 1 / settings.INPUT_SAMPLE_RATE)
    times, history = create_spectrogram(len(xf))
    plotter.spectrogram(times, xf, history, 'spectrogram_normal', fmax=fmax, fmin=fmin)

    # data = banding(sample_rate, audio)
    # plotter.spectrogram(*data, f'spectogram_compression', fmax=fmax, fmin=fmin)


if __name__ == '__main__':
    test()
