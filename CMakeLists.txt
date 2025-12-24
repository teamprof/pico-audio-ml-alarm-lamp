# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
        set(USERHOME $ENV{USERPROFILE})
else()
        set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)

if(EXISTS ${picoVscode})
        include(${picoVscode})
endif()

# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# ====================================================================================
project(app C CXX ASM)

if(PICO_SDK_VERSION_STRING VERSION_LESS "2.1.0")
        message(FATAL_ERROR "Raspberry Pi Pico SDK version 2.1.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# If you want debug output from USB (pass -DPICO_STDIO_USB=1) this ensures you don't lose any debug output while USB is set up
if(NOT DEFINED PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS)
        set(PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS 3000)
endif()

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# ====================================================================================
# Define ARM_CPU, CMSIS ROOT and DSP to use CMSIS-DSP
set(ARM_CPU "cortex-m0plus")

set(CONFIGTABLE ON)
set(RFFT_Q15_256 ON)
set(ALLFAST ON)

set(ROOT ${CMAKE_CURRENT_LIST_DIR}/lib/CMSIS_5)
set(DSP ${ROOT}/CMSIS/DSP)
include(${DSP}/Toolchain/GCC.cmake)
add_subdirectory(${DSP}/Source EXCLUDE_FROM_ALL)

# add pico tensorflow lite mcu lib
set(TFLMICRO ${CMAKE_CURRENT_LIST_DIR}/lib/pico-tflmicro)
add_subdirectory(${TFLMICRO} EXCLUDE_FROM_ALL)

# ====================================================================================
# Add executable. Default name is the project name, version 0.1
add_executable(app
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/dsp_pipeline.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ml_model.cpp

    ${CMAKE_CURRENT_LIST_DIR}/src/ws2812/ws2812.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/audio/i2s.cpp
)


pico_set_program_name(app "app")
pico_set_program_version(app "1.0.0")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(app 0)
pico_enable_stdio_usb(app 1)

target_compile_definitions(app PRIVATE
        DPICO_STDIO_USB=1
)


# Add the standard library to the build
target_link_libraries(app
        pico_atomic
        pico_stdio_usb
        pico_stdlib
        hardware_clocks
        hardware_dma
        hardware_pio
        hardware_pwm

        CMSISDSP
        pico-tflmicro
)

pico_add_extra_outputs(app)
