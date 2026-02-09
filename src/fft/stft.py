import sys
import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile
from scipy.signal import ShortTimeFFT, windows, find_peaks

from src.fft import plotter
from src.profiler import Timer
timer = Timer('fft', 1)

np.set_printoptions(threshold=sys.maxsize)

def stft(
    audio: np.ndarray,
    fs: float,
    window_size: int = 1024,
    hop: int = 256,
    fmax: float | None = None,
):
    """
    Plot a spectrogram using SciPy's ShortTimeFFT.

    Parameters
    ----------
    audio : np.ndarray
        1-D audio signal
    fs : float
        Sampling rate (Hz)
    window_size : int
        FFT/window length
    hop : int
        Hop size between frames
    fmax : float or None
        Optional upper frequency limit for plotting
    """

    # --- window ---
    win = windows.hann(window_size, sym=False)

    # --- STFT object ---
    stft = ShortTimeFFT(
        win=win,
        hop=hop,
        fs=fs,
        mfft=window_size,
        scale_to='magnitude'
    )

    # --- compute transform ---
    S = stft.stft(audio)

    # magnitude → dB (numerical stability offset)
    # S_db = 20 * np.log10(np.abs(S) + 1e-12)
    S_db = np.abs(S)

    # axes
    times = stft.t(len(audio))
    freqs = stft.f

    # optional frequency limit
    if fmax is not None:
        mask = freqs <= fmax
        freqs = freqs[mask]
        S_db = S_db[mask, :]

    return times, freqs, S_db

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

def main():
    sample_rate, wav = wavfile.read('/home/pi/repositories/led_strip/assets/BotW - Item.wav')

    mono = wav.mean(axis=1)


    timer.start()
    data = stft(mono, sample_rate, window_size=1024, fmax=2500)
    timer.end('stft')
    print(data[0].shape)
    print(data[1].shape)
    print(data[2].shape)

    timer.start()
    spectrogram = plotter.specrogram(*data)
    plotter.save_plot(spectrogram, 'stft_spectrogram')
    timer.end('saving')

    # timer.start()
    # data = filter_peaks(*data)
    # timer.end('filter_peaks')

    # timer.start()
    # spectrogram = plotter.specrogram(*data)
    # plotter.save_plot(spectrogram, "peaks")
    # timer.end('saving')

    # data_left = plot_stft_spectrogram(wav[:,0], sample_rate, fmax=2500)
    # data_right = plot_stft_spectrogram(wav[:,1], sample_rate, fmax=2500)
    # save_plot(*data_left, name = 'left')
    # save_plot(*data_right, name = 'right')
