cmake_minimum_required(VERSION 3.12)

if (NOT DEFINED PICO_SDK_PATH)
    if (NOT $ENV{PICO_SDK_PATH} STREQUAL "")
        set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
    else()
        message(FATAL_ERROR "Please set PICO_SDK_PATH")
    endif()
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(circle C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(${PROJECT_NAME} main.c ../../src/pcd8544.c)

target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_spi)

pico_add_extra_outputs(${PROJECT_NAME})
endif()
