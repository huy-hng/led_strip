import traceback

# from src.main import main
from src.fft import fft, stft, fft_testing

if __name__ == '__main__':
    try:
        # main()
        fft_testing.main()
        # stft.main()
        input('Press any key to continue.')
    # except KeyboardInterrupt:
    #     strip_man.clear()

    except Exception as e:
        print()
        traceback.print_exc()
        print()
        input('Press any key to continue.')



    # with cProfile.Profile() as pr:
    #     pr.disable()
    #     main()

    # profiling_stats(pr, 'normal')
    # strip_man.clear()
