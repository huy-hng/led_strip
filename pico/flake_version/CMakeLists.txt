cmake_minimum_required(VERSION 3.13)
project(pico2w_minimal C CXX ASM)

set(PICO_TARGET_CPU rp2350)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
pico_sdk_init()

add_executable(pico2w_minimal main.c)
target_link_libraries(pico2w_minimal pico_stdlib)
pico_enable_stdio_usb(pico2w_minimal 1)
pico_enable_stdio_uart(pico2w_minimal 0)
pico_add_extra_outputs(pico2w_minimal)
