import time
from functools import wraps
from contextlib import ContextDecorator
from typing import Literal
from dataclasses import dataclass

from src import global_variables as globals

def get_time_unit(time_taken):
    unit = 's'
    if time_taken < 1:
        time_taken *= 1000
        unit = 'ms'
    return time_taken, unit

def print_every_x_times(msg, x=1):
    if globals.step % x == 0:
        print(msg)

@dataclass
class _Timer(ContextDecorator):
    name: str | None
    time_between_prints: float
    average_steps: int
    print_strategy: str
    once: bool


    total_time: float = 0
    averaged_time: float = 0

    last_print = time.perf_counter()

    step_at_last_print: int = 0
    step: int = 0
    callback = None

    def _recreate_cm(self):
        return self

    def __call__(self, func):
        if self.callback is not None:
            self.name = func.__name__
            self.callback(func.__name__, self)

        @wraps(func)
        def inner(*args, **kwds):
            with self._recreate_cm():
                return func(*args, **kwds)
        return inner

    def __enter__(self):
        self.start_time = time.perf_counter()

    def __exit__(self, *exc):
        end_time = time.perf_counter()
        self.averaged_time += end_time - self.start_time

        self.step += 1

        if self.once:
            if self.step > 1: return
            else: self.print_time(end_time - self.start_time)
            return

        time_taken = None
        if self.print_strategy == 'time' and end_time - self.last_print > self.time_between_prints:
            time_taken = self.averaged_time / (self.step - self.step_at_last_print)

        if self.print_strategy == 'steps' and self.step % self.average_steps == 0:
            time_taken = self.averaged_time / self.average_steps


        if time_taken is not None:
            self.last_print = end_time
            self.print_time(time_taken)


    def print_time(self, time_taken):

        self.total_time += self.averaged_time
        self.averaged_time = 0
        self.step_at_last_print = self.step

        time_taken, unit = get_time_unit(time_taken)
        total_time, total_unit = get_time_unit(self.total_time)

        name = ''
        if self.name is not None:
            name = f'{self.name}: '
        print(f'{name}{time_taken:.2f}{unit} (cum: {total_time:.2f}{total_unit})')

class Timer:
    timers: dict = {}
    print_strategy: Literal['time', 'steps']
    def __new__(cls, name=None, time_between_prints=0, average_steps=1, print_strategy='time', once=False) -> _Timer:
        if name in cls.timers:
            return cls.timers[name]

        timer = _Timer(name, time_between_prints, average_steps, print_strategy, once)

        if name is None or name == '':
            timer.callback = cls.save_timer
            return timer

        cls.timers[name] = timer

        return timer

    @classmethod
    def save_timer(cls, name, timer):
        cls.timers[name] = timer

    def __enter__(self): ...
    def __exit__(self, *exc): ...
    def __call__(self, name): ...


if __name__ == '__main__':
    loops = 100
    # average = 10

    @Timer(time_between_prints=0.4)
    def test_fn():
        time.sleep(0.01)

    @Timer(time_between_prints=1)
    def test_fn1():
        time.sleep(0.01)

    start = time.perf_counter()
    for _ in range(loops):
        test_fn()
        test_fn1()
        # with Timer('with', average):
        #     time.sleep(0.002)

    print(f'actual time: {(time.perf_counter()-start)/loops*1000:.2f}ms')
    print(f'total time: {(time.perf_counter()-start):.2f}s')
