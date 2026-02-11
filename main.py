import cProfile
import pstats
import traceback

from src.settings import project_path
from src import settings
import importlib

def testing():
    from src.fft import fft_testing
    fft_testing.test()

def led():
    from src.led import run
    run()

def profiling_stats(pr, name):
    stats = pstats.Stats(pr)
    stats.sort_stats(pstats.SortKey.TIME)
    stats.dump_stats(filename=f'{project_path}/logs/{name}.prof')
    # stats.print_stats()

if __name__ == '__main__':
    print('------------------------------------starting-------------------------------------')

    try:
        # testing()
        with cProfile.Profile() as pr:
            # pr.disable()
            led()

    except Exception as _:
        print()
        traceback.print_exc()
        print()
    finally:
        input('Press any key to continue.')
        profiling_stats(pr, 'normal')
