import cProfile
import pstats
import traceback

from src.settings import project_path

def test_fft():
    from src.fft import fft_testing
    fft_testing.test()

def lights():
    from src.lights.led import run
    run()

def test_lights():
    from src.lights.test import run
    run()

def profiling_stats(pr, name):
    stats = pstats.Stats(pr)
    stats.sort_stats(pstats.SortKey.TIME)
    stats.dump_stats(filename=f'{project_path}/logs/{name}.prof')

if __name__ == '__main__':
    print('------------------------------------starting-------------------------------------')

    try:
        with cProfile.Profile() as pr:
            pr.disable()
            # testing()
            lights()
            # test_lights()

    except Exception as _:
        print()
        traceback.print_exc()
        print()
        input('Press any key to continue.')
    finally:
        profiling_stats(pr, 'lights')
