cmake_minimum_required(VERSION 4.1)

include(pico_sdk_import.cmake)

project(blink C CXX ASM)
# set(CMAKE_C_STANDARD "/run/current-system/sw/bin/gcc")
# set(CMAKE_CXX_STANDARD "/run/current-system/sw/bin/g++")

add_executable(blink blink.c)

pico_sdk_init()

# pull in common dependencies
target_link_libraries(blink pico_stdlib)

if (PICO_CYW43_SUPPORTED)
    target_link_libraries(blink pico_cyw43_arch_none)
endif()

# create map/bin/hex file etc.
pico_add_extra_outputs(blink)

# add url via pico_set_program_url
example_auto_set_url(blink)
