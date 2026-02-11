import bisect

from src.util import Timer

from numba import njit, prange
import numpy as np
from scipy.fft import rfft, rfftfreq

with Timer('find_peaks'):
    from scipy.signal import find_peaks


from src.fft import notes
from src import settings
from src.settings import EPSILON

def create_updater(window_size=settings.FFT_WINDOW_SIZE):
    buffer = np.zeros(window_size)
    def update(new_samples):
        nonlocal buffer
        buffer = np.roll(buffer, -len(new_samples))
        buffer[-len(new_samples):] = new_samples

        yf = rfft(buffer)

        return np.abs(yf)
    return update

update = create_updater()

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
    notes.piano_frequencies
)

@Timer(once=True)
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

def fft_pipeline(new_samples):
    global current_peak
    data = update(new_samples)

    # logarithmic compression
    # data = np.log1p(data)
    # data = 20*np.log10(data + EPSILON)

    data = map_bands(data)

    data = filter_peaks(data)

    # peak tracking
    current_peak = max(data.max(), current_peak * settings.FFT_PEAK_DECAY)

    # normalize
    data = data / (current_peak + EPSILON)

    # noise gate
    data[data < settings.FFT_NOISE_GATE] = 0

    # map to 255
    data = np.clip(data * 255, 0, 255).astype(np.uint8)
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
