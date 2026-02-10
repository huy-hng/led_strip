import traceback

from src.profiler import Timer
timer = Timer('main', 1)


def testing():
    from src.fft import fft_testing
    fft_testing.test()

def main():
    from src.led import run
    run()


if __name__ == '__main__':
    print('starting')
    try:
        testing()
        # main()
    except Exception as _:
        print()
        traceback.print_exc()
        print()
    finally:
        input('Press any key to continue.')




    # with cProfile.Profile() as pr:
    #     pr.disable()
    #     main()

    # profiling_stats(pr, 'normal')
    # strip_man.clear()
