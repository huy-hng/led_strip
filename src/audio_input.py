import time
import numpy as np
import pyaudio
from pyaudio import PyAudio, paInt16
from src.fft.dsp import fft_pipeline
from src import settings
from src.util import Timer

stream: PyAudio.Stream

def stream_callback(callback):
    def wrapper(in_data, frame_count, time_info, status):
        global stream
        buffer_array = np.frombuffer(in_data, dtype=np.int16)
        should_continue = callback(buffer_array)

        # if not should_continue:
        #     stream.stop_stream()
        #     stream.close()
        #     p.terminate()


        c = pyaudio.paContinue if should_continue else pyaudio.paAbort
        return None, c
    return wrapper

def start_stream(callback) -> PyAudio.Stream:
    global stream

    p = PyAudio()
    print('---------------------------------------------------------------------------------')

    # frames_per_buffer = int(settings.INPUT_SAMPLE_RATE / settings.FPS)
    frames_per_buffer = int(settings.FFT_HOP_SIZE)

    stream = p.open(format=paInt16,
                    channels=1,
                    rate=settings.INPUT_SAMPLE_RATE,
                    input=True,
                    frames_per_buffer=frames_per_buffer,
                    stream_callback=stream_callback(callback))

    # while True:
        # print(stream.get_read_available())
        # with Timer('stream loop', 100):
            # buffer = stream.read(frames_per_buffer, exception_on_overflow=False)
            # buffer = stream.read(max(1, stream.get_read_available()), exception_on_overflow=False)
        # buffer_array = np.frombuffer(buffer, dtype=np.int16)
        # yield callback(buffer_array)

    return stream
