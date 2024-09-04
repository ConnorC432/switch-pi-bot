cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_STANDARD_REQUIRED ON)
set(FAMILY rp2040)
set(BOARD pico_sdk)

project(RP2040 C CXX ASM)

#Initialize the Pico SDK
include(pico_sdk_import.cmake)
pico_sdk_init()

add_executable(RP2040
        src/main.cpp
)

target_link_libraries(RP2040
)

target_include_directories(RP2040 PRIVATE
)

pico_enable_stdio_usb(RP2040 1)
pico_enable_stdio_uart(RP2040 0)

#UF2 Output
pico_add_extra_outputs(RP2040)