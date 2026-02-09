import numpy as np
import matplotlib.pyplot as plt

from src.profiler import Timer
timer = Timer('fft', 1)

def save_plot(plot, name):
    path = '/home/pi/repositories/led_strip/assets/fft_output/'
    plot.savefig(path + name + '.png')


def simple_plot(xf, yf, freq_range=None):
    plt.figure(figsize=[34.4, 5], dpi=100)
    plt.plot(xf, np.abs(yf)) # type: ignore

    if freq_range:
        ax = plt.gca()
        # ax.set_xscale('log')
        ax.set_xlim(left=freq_range[0], right=freq_range[1])
        # ax.xaxis.set_ticks(frequencies[start_note:end_note+1])

    return plt


def spectrogram(times, freqs, S_db, fname, fmax=None):
    timer.start()

    plt.figure(figsize=(10, 6))

    plt.pcolormesh(
        times,
        freqs,
        S_db,
        shading="gouraud"
    )

    plt.title("STFT Spectrogram")
    plt.xlabel("Time (s)")
    plt.ylabel("Frequency (Hz)")
    plt.colorbar(label="Magnitude (dB)")

    if fmax:
        ax = plt.gca()
        ax.set_ylim(top=fmax)

    plt.tight_layout()
    timer.end('plotting')

    timer.start()
    save_plot(plt, fname)
    timer.end('saving')
    return plt

def multiple_spectrogram(data, fname='spectrogram', fmax=None):
    # plt.figure(figsize=(20, 20))

    plt.title("Spectrogram")
    # plt.colorbar(label="Magnitude (dB)")

    # subplots = []
    for d in data:
        fig, ax = plt.subplots()
        fig.set_size_inches(10, 10)

        ax.pcolormesh(*d[0], shading="gouraud")

        ax.set_title(d[1])
        ax.set_xlabel('Time (s)')
        ax.set_ylabel('Frequency (Hz)')

        if fmax: ax.set_ylim(top=fmax)

        # subplots.append(fig)

    plt.tight_layout()

    save_plot(plt, fname)
    return plt




# fig1, ax1 = plt.subplots()
# fig2, ax2 = plt.subplots()

# ax1.plot(ages, dev_salaries, color='#444444',
#          linestyle='--', label='All Devs')

# ax2.plot(ages, py_salaries, label='Python')
# ax2.plot(ages, js_salaries, label='JavaScript')

# ax1.legend()
# ax1.set_title('Median Salary (USD) by Age')
# ax1.set_ylabel('Median Salary (USD)')

# ax2.legend()
# ax2.set_xlabel('Ages')
# ax2.set_ylabel('Median Salary (USD)')

# plt.tight_layout()

# plt.show()

# fig1.savefig('fig1.png')
# fig2.savefig('fig2.png')
