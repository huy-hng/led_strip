import bisect

from src.util import Timer

from numba import njit, prange
import numpy as np
from scipy.fft import rfft, rfftfreq

from scipy.signal import find_peaks

from src import settings
from src.settings import EPSILON

def create_fft(window_size=settings.FFT_WINDOW_SIZE):
    buffer = np.zeros(window_size)
    def fft(new_samples):
        nonlocal buffer
        buffer = np.roll(buffer, -len(new_samples))
        buffer[-len(new_samples):] = new_samples

        yf = rfft(buffer)

        return np.abs(yf)
    return fft

fft = create_fft()

current_peak = settings.FFT_STARTING_PEAK


def create_band_map(sample_rate, window_size, band_centers):
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
    freqs = rfftfreq(window_size, 1/sample_rate)
    band_centers = np.asarray(band_centers)

    # --- compute band edges ---
    edges = np.zeros(len(band_centers) + 1)

    edges[1:-1] = np.sqrt(
        band_centers[:-1] * band_centers[1:]
    )

    # extrapolate first/last edge
    edges[0] = band_centers[0]**2 / edges[1]
    edges[-1] = band_centers[-1]**2 / edges[-2]

    return freqs, band_centers, edges

fft_freqs, band_centers, band_edges = create_band_map(
    settings.INPUT_SAMPLE_RATE,
    settings.FFT_WINDOW_SIZE,
    settings.FREQUENCY_RANGE
)

@Timer(time_between_prints=settings.TIME_BETWEEN_PRINTS)
@njit(parallel=True, fastmath=True, cache=True)
def map_bands(mag) -> np.ndarray:
    # --- aggregate bins ---
    band_values = np.zeros(len(band_centers))

    for i in prange(len(band_centers)): # ty: ignore
        mask = (fft_freqs >= band_edges[i]) & (fft_freqs < band_edges[i+1])

        if np.any(mask):
            band_values[i] = mag[mask].mean()
        else:
            band_values[i] = 0

    return band_values

# @Timer(time_between_prints=settings.TIME_BETWEEN_PRINTS)
def fft_pipeline(new_samples):
    global current_peak

    with Timer('fft', time_between_prints=settings.TIME_BETWEEN_PRINTS):
        data = fft(new_samples)

    data[data < settings.FFT_MIN_ACTIVATION] = 0 # noise gate
    data = map_bands(data)


    with Timer('fft normalize', time_between_prints=settings.TIME_BETWEEN_PRINTS):
        current_peak = max(data.max(), current_peak * settings.FFT_PEAK_DECAY) # peak tracking
        data = data / (current_peak + EPSILON) # normalize
        data[data < settings.FFT_NOISE_GATE] = 0 # noise gate
        data = np.clip(data * 255, 0, 255).astype(np.uint16) # map to 255

    data = filter_peaks(data)

    return data

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
    freqs = map_to_closest(settings.FREQUENCY_RANGE, xf)

    transformed = np.zeros(len(freqs))
    region_factor = 30
    for i, f in enumerate(freqs):
        start = -int(f[1]/region_factor) + f[0]
        end = int(f[1]/region_factor) + f[0]
        transformed[i] = sum(yf[start:end])

    transformed *= 255 / transformed.max()

@Timer(time_between_prints=settings.TIME_BETWEEN_PRINTS)
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
