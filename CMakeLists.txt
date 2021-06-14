cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(farm-a1-temp-control C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(farm-a1-temp-control
    main.cpp    
    )

target_link_libraries(farm-a1-temp-control pico_stdlib hardware_adc)

pico_enable_stdio_usb(farm-a1-temp-control 1)
pico_enable_stdio_uart(farm-a1-temp-control 0)

pico_add_extra_outputs(farm-a1-temp-control)
