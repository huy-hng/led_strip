import time
import numpy as np
import pyaudio
from pyaudio import PyAudio, paInt16
from src import settings
from src.util import Timer

print()
print()
p = PyAudio()
print()
print()

# frames_per_buffer = int(settings.INPUT_SAMPLE_RATE / settings.FPS)
# frames_per_buffer = int(settings.FFT_HOP_SIZE)
frames_per_buffer = 256
stream: PyAudio.Stream = p.open(format=paInt16,
                                channels=1,
                                rate=settings.INPUT_SAMPLE_RATE,
                                input=True,
                                frames_per_buffer=frames_per_buffer
                                )

print(f'{frames_per_buffer=}')
def start_stream(callback):
    while True:
            # print(stream.get_read_available())
        with Timer('stream loop', 100):
            buffer = stream.read(frames_per_buffer, exception_on_overflow=False)
            # buffer = stream.read(max(1, stream.get_read_available()), exception_on_overflow=False)
        buffer_array = np.frombuffer(buffer, dtype=np.int16)
        yield callback(buffer_array)


    stream.stop_stream()
    stream.close()
    p.terminate()
