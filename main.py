import cProfile
import pstats
import traceback

def testing():
    from src.fft import fft_testing
    fft_testing.test()

def main():
    from src.led import run
    run()

def profiling_stats(pr, name):
    stats = pstats.Stats(pr)
    stats.sort_stats(pstats.SortKey.TIME)
    stats.dump_stats(filename=f'/home/pi/repositories/led_strip/logs/{name}.prof')
    # stats.print_stats()

if __name__ == '__main__':
    print('starting')
    try:
        with cProfile.Profile() as pr:
            pr.disable()
            testing()
            # main()
        profiling_stats(pr, 'normal')

    except Exception as _:
        print()
        traceback.print_exc()
        print()
    # finally:
        input('Press any key to continue.')

