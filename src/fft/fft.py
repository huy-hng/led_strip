import bisect
from src.fft import notes

import numpy as np
from src import settings
from scipy.fft import rfft
from scipy.signal import find_peaks

def create_updater(window_size=settings.FFT_WINDOW_SIZE):
    buffer = np.zeros(window_size)
    def update(new_samples):
        nonlocal buffer
        buffer = np.roll(buffer, -len(new_samples))
        buffer[-len(new_samples):] = new_samples

        yf = rfft(buffer)

        return np.abs(yf) # type: ignore
    return update

update = create_updater()


def map_fft_to_log_bands(mag, fs, N, band_centers) -> np.ndarray:
    """
    Map FFT magnitudes into logarithmic frequency bands.

    Parameters
    ----------
    mag : array
        rFFT magnitude spectrum
    fs : float
        sampling rate
    N : int
        FFT size
    band_centers : array-like
        center frequencies of bands

    Returns
    -------
    band_values : np.ndarray
        aggregated magnitude per band
    """

    freqs = np.fft.rfftfreq(N, 1/fs)
    band_centers = np.asarray(band_centers)

    # --- compute band edges ---
    edges = np.zeros(len(band_centers) + 1)

    edges[1:-1] = np.sqrt(
        band_centers[:-1] * band_centers[1:]
    )

    # extrapolate first/last edge
    edges[0] = band_centers[0]**2 / edges[1]
    edges[-1] = band_centers[-1]**2 / edges[-2]

    # --- aggregate bins ---
    band_values = np.zeros(len(band_centers))

    for i in range(len(band_centers)):
        mask = (freqs >= edges[i]) & (freqs < edges[i+1])

        if np.any(mask):
            band_values[i] = mag[mask].mean()
        else:
            band_values[i] = 0

    return band_values


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

def filter_by_notes(yf, xf):
    frequencies = map_to_closest(notes.piano_frequencies, xf)

    transformed = np.zeros(len(frequencies))
    region_factor = 30
    for i, f in enumerate(frequencies):
        start = -int(f[1]/region_factor) + f[0]
        end = int(f[1]/region_factor) + f[0]
        transformed[i] = sum(yf[start:end]) # type: ignore

    transformed *= 255 / transformed.max()

    # plot = plotter.simple_plot([freq for _, freq in frequencies], transformed)
    # plotter.save_plot(plot, 'fft')

def filter_peaks(yf) -> np.ndarray:
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

    # peak_freqs = xf[peaks]
    # peak_mags  = mag[peaks]

    mags = np.zeros_like(yf)
    mags[peaks] = mag[peaks]
    return mags
