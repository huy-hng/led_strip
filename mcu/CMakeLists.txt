cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico2_w)
set(PICO_PLATFORM rp2350-arm-s)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico2w_led C CXX ASM)

pico_sdk_init()

add_executable(pico2w_led
	src/main.cpp
	src/dma.cpp
	src/util.cpp
	src/visualizer.cpp
	src/adc.cpp
)

if (PICO_CYW43_SUPPORTED)
	target_link_libraries(pico2w_led pico_cyw43_arch_none)
endif()

target_link_libraries(pico2w_led
	pico_stdlib
	hardware_adc
)

pico_enable_stdio_usb(pico2w_led 1)
pico_enable_stdio_uart(pico2w_led 0)
pico_add_extra_outputs(pico2w_led)
