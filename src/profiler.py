import time
from functools import wraps
from functools import wraps
from typing import Callable



class Timer:
    enable = True
    combine_results: int
    start_time: float
    timer: float = 0
    step = 0
    def __init__(self, name: str, combine_results = 1) -> None:
        self.name = name
        self.combine_results = combine_results

    def start(self):
        if not self.enable: return
        self.start_time = time.perf_counter()
        self.step += 1

    def end(self, name=''):
        if not self.enable: return
        self.timer += time.perf_counter() - self.start_time
        self.print_time(name)

    def dec(self, func: Callable) -> Callable:
        @wraps(func)
        def wrapper(*args, **kwargs):
            start_time = time.perf_counter()

            result = func(*args, **kwargs)

            self.timer += time.perf_counter() - start_time

            self.print_time(func.__name__)

            self.step += 1
            return result

        return wrapper


    def print_time(self, name=''):
        if self.step % self.combine_results == 0:
            if name:
                name = f'.{name}'


            time_taken = self.timer/self.combine_results
            unit = 's'
            if time_taken < 0:
                time_taken *= 1000
                unit = 'ms'

            print(f'{self.name}{name}: {time_taken:.2f}{unit}')
            self.timer = 0
