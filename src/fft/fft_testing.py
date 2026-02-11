import sys
import math
import time
from itertools import zip_longest

import numpy as np
from scipy.io import wavfile
from scipy.signal import find_peaks

from src import settings
from src.settings import project_path
# from src.fft import plotter, dsp
from src.fft import dsp
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
    # audio_file = 'Frederic_Chopin_-_Nocturne_Eb_major_Opus_9,_number_2.wav'
    audio_file = f'{project_path}/assets/song.wav'

    sample_rate, wav = wavfile.read(audio_file)
    # sample_duration = len(mono) / sample_rate
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


def get_mock_audio(sample_rate, audio, wait=True):
    chunks = chunker(audio, int(settings.FFT_HOP_SIZE), 0)
    wait_time = settings.FFT_HOP_SIZE / sample_rate
    start_time = time.perf_counter()

    while True:
        if wait and time.perf_counter() - start_time < wait_time:
            yield
            continue

        start_time = time.perf_counter()

        try:
            yield next(chunks)
        except StopIteration:
            if not wait: return
            return

@Timer()
def banding(sample_rate, audio):
    num_windows = math.ceil(len(audio)/settings.FFT_HOP_SIZE)

    bands_history = np.zeros((num_windows, len(settings.piano_frequencies)))

    for i, bands in enumerate(get_mock_audio(sample_rate, audio, wait=False)):
        bands_history[i] = bands

    bands_history = np.swapaxes(bands_history, 0, 1)

    times = [(settings.FFT_HOP_SIZE / sample_rate) * i for i in range(num_windows)]
    return times, settings.piano_frequencies, bands_history



@Timer('get_real_data')
def get_real_data(len_frequencies, samples_to_record):
    num_windows = math.ceil(samples_to_record/settings.FFT_HOP_SIZE)

    history = np.zeros((num_windows, len_frequencies))
    i = 0

    def get_data(buffer_array: np.ndarray):
        nonlocal i
        fft_data = dsp.fft_pipeline(buffer_array)

        if i % int(num_windows/8) == 0: print(fft_data)

        if i == num_windows: return False
        history[i] = fft_data
        i += 1
        return True

    stream = start_stream(get_data)

    with Timer('stream loop'):
        while stream.is_active():
            time.sleep(0.01)


    # for i, yf in enumerate(start_stream(dsp.fft_pipeline)):
    #     if i == num_windows: break
    #     history[i] = yf

    history = np.swapaxes(history, 0, 1)

    times = [(settings.FFT_HOP_SIZE / settings.INPUT_SAMPLE_RATE) * i for i in range(num_windows)]
    return times, history


def get_fake_data(len_frequencies, samples_to_record):
    sample_rate, audio = get_audio()

    num_windows = math.ceil(samples_to_record/settings.FFT_HOP_SIZE)

    history = np.zeros((num_windows, len_frequencies))

    for i, chunk in enumerate(get_mock_audio(sample_rate, audio, False)):
        if i == num_windows: break
        data = dsp.fft_pipeline(chunk)
        history[i] = data

    history = np.swapaxes(history, 0, 1)

    times = [(settings.FFT_HOP_SIZE / settings.INPUT_SAMPLE_RATE) * i for i in range(num_windows)]
    return times, history


def test():
    freq_range = settings.FREQUENCY_RANGE
    fmin = settings.piano_frequencies[0]
    fmax = settings.piano_frequencies[-1]
    # fmin = 0
    # fmax = 8000

    seconds_to_record = 4
    samples_to_record = seconds_to_record * settings.INPUT_SAMPLE_RATE

    times, history = get_real_data(len(freq_range), samples_to_record)
    fname = f'rate={settings.INPUT_SAMPLE_RATE}, win={settings.FFT_WINDOW_SIZE}, hop={settings.FFT_HOP_SIZE}, 88freqs'
    # fname = f'sample_rate={settings.INPUT_SAMPLE_RATE}'
    # plotter.spectrogram(times, freq_range, history, fname, fmax=fmax, fmin=fmin)

    # times, history = get_fake_data(len(freq_range), samples_to_record)
    # plotter.spectrogram(times, freq_range, history, 'fake_data', fmax=fmax, fmin=fmin)
