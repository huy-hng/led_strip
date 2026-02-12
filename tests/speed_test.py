from concurrent.futures import ThreadPoolExecutor

def test_speed():
    with ThreadPoolExecutor() as ex:
        pass

if __name__ == '__main__':
    import timeit
    import time
    start = time.perf_counter()
    print(timeit.timeit("test_speed()", setup="from __main__ import test_speed"))
    print(time.perf_counter() - start)
