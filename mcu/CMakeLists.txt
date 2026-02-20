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
	src/dsp.cpp
)

if (PICO_CYW43_SUPPORTED)
	target_link_libraries(pico2w_led pico_cyw43_arch_none)
endif()

target_compile_options(pico2w_led PRIVATE
    -mcpu=cortex-m33
    -mfpu=fpv5-sp-d16
	-mfloat-abi=softfp
	# -O3
	-Ofast
	# -Wsign-compare
 #    -Wdouble-promotion
 #    -ffast-math
 #    -DNDEBUG
 #    -Wall
	# -Wextra
	# -Werror
 #    -fshort-enums
    # -fshort-wchar
)

add_subdirectory(lib/CMSIS-DSP)
# add_library(CMSISDSP STATIC)

set(DSP_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/CMSIS-DSP)
set(DSP_PATH_SOURCE ${CMAKE_CURRENT_LIST_DIR}/lib/CMSIS-DSP/Source)



# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/CommonTables/arm_common_tables.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/CommonTables/arm_common_tables_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/CommonTables/arm_const_structs.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/CommonTables/arm_const_structs_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_cos_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_cos_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_cos_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_sin_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_sin_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_sin_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_sqrt_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_sqrt_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_vlog_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_vlog_f64.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_vexp_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_vexp_f64.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_vlog_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_vlog_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_vlog_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_vexp_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_vinverse_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_atan2_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_divide_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_divide_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_atan2_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_atan2_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/FastMathFunctions/arm_atan2_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_bitreversal.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_bitreversal2.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_bitreversal_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix2_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix8_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_init_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix2_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_init_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_init_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_f64.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_init_f64.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix2_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_init_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix2_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_init_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_init_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_rfft_init_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_rfft_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_init_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_init_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_rfft_init_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_rfft_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_init_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_init_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_rfft_fast_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_rfft_fast_init_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_init_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix8_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_rfft_fast_f64.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_rfft_fast_init_f64.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_rfft_fast_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_rfft_fast_init_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_init_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix8_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_init_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_rfft_init_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_rfft_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_init_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_rfft_init_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_rfft_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_init_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix2_init_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix2_init_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_init_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix4_init_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix2_init_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_cfft_radix2_init_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_mfcc_init_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_mfcc_f32.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_mfcc_init_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_mfcc_q31.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_mfcc_init_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_mfcc_q15.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_mfcc_init_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_mfcc_f16.c)
# target_sources(CMSISDSP PRIVATE ${DSP_PATH_SOURCE}/TransformFunctions/arm_transform_buffer_sizes.c)


target_include_directories(CMSISDSP PUBLIC
	# ${DSP_PATH}/Include
	${PICO_SDK_PATH}/src/rp2_common/cmsis/stub/CMSIS/Core/Include
	${DSP_PATH}/PrivateInclude
)


target_compile_definitions(CMSISDSP PUBLIC
    ARM_MATH_CM33
    ARM_MATH_DSP
	DISABLEFLOAT64 # since pico2w has single precision FPU only
	__FPU_PRESENT=1
)


target_link_libraries(pico2w_led
	pico_stdlib
	hardware_adc
	# cmsis_core
    CMSISDSP
)


pico_enable_stdio_usb(pico2w_led 1)
pico_enable_stdio_uart(pico2w_led 0)
pico_add_extra_outputs(pico2w_led)
