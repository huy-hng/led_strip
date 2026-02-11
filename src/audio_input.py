import time
import numpy as np
import pyaudio
from src import settings
from src.util import Timer

print()
print()
p = pyaudio.PyAudio()
print()
print()

frames_per_buffer = int(settings.INPUT_SAMPLE_RATE / settings.FPS)
stream = p.open(format=pyaudio.paInt16,
                channels=1,
                rate=settings.INPUT_SAMPLE_RATE,
                input=True,
                frames_per_buffer=frames_per_buffer
                )

def start_stream(callback):
    overflows = 0
    prev_ovf_time = time.time()
    while True:
        try:
            y = np.frombuffer(stream.read(frames_per_buffer, exception_on_overflow=False), dtype=np.int16)
            y = y.astype(np.float32)
            stream.read(stream.get_read_available(), exception_on_overflow=False)

            yield callback(y)

        except IOError:
            overflows += 1
            if time.time() > prev_ovf_time + 1:
                prev_ovf_time = time.time()
                print('Audio buffer has overflowed {} times'.format(overflows))

    stream.stop_stream()
    stream.close()
    p.terminate()
