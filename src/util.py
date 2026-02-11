import time
from functools import wraps
from contextlib import ContextDecorator

class _Timer(ContextDecorator):
    name: str | None
    # time_between_prints: float = 0
    average_results: int

    start_time: float
    step: int = 0
    combined_time: float = 0
    callback = None

    def _recreate_cm(self):
        return self

    def __call__(self, func):
        if self.callback is not None:
            self.callback(func.__name__, self)

        @wraps(func)
        def inner(*args, **kwds):
            with self._recreate_cm():
                return func(*args, **kwds)
        return inner

    def __init__(self, name, average_results, once):
        self.name = name
        self.average_results = average_results
        self.once = once

    def __enter__(self):
        self.start_time = time.perf_counter()

    def __exit__(self, *exc):
        self.combined_time += time.perf_counter() - self.start_time

        self.step += 1

        if self.once is False and self.step % self.average_results == 0:
            self.print_time()
            return

        if self.once:
            if self.step > 1: return
            else: self.print_time()



    def print_time(self):
        time_taken = self.combined_time / self.average_results
        self.combined_time = 0

        unit = 's'
        if time_taken < 1:
            time_taken *= 1000
            unit = 'ms'

        name = ''
        if self.name is not None:
            name = f'{self.name}: '
        print(f'{name}{time_taken:.2f}{unit} on average for {self.step} steps')

class Timer:
    timers: dict = {}
    def __new__(cls, name=None, average_results=1, once=False) -> _Timer:
        if name in cls.timers:
            return cls.timers[name]

        timer = _Timer(name, average_results, once)

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
    loops = 10
    average = 10

    @Timer()
    def test_fn():
        time.sleep(0.001)

    @Timer()
    def test_fn1():
        time.sleep(0.001)

    print(Timer.__dict__)
    # start = time.perf_counter()
    # for _ in range(loops):
    #     test_fn()
    #     with Timer('with', average):
    #         time.sleep(0.002)

    # print(f'actual time: {(time.perf_counter()-start)/loops*1000:.2f}ms')
