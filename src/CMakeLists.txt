cmake_minimum_required (VERSION 3.18)

# Set the project name
set(APP_NAME CubeSat)
set(CXX_STANDARD 20)

project (${APP_NAME})

add_executable(CubeSat)

target_compile_options(CubeSat PRIVATE -pthread)
target_link_options(CubeSat PRIVATE -pthread)

target_include_directories(CubeSat PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/sensors
    ${CMAKE_CURRENT_SOURCE_DIR}/logging
    ${CMAKE_CURRENT_SOURCE_DIR}/external/a3g4250d-pid
    ${CMAKE_CURRENT_SOURCE_DIR}/external/lis2dw12-pid
    ${CMAKE_CURRENT_SOURCE_DIR}/external/lps22hb-pid
)

target_sources(CubeSat PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/cubesat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/i2c-linux.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/a3g4250d-pid/a3g4250d_reg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/lis2dw12-pid/lis2dw12_reg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/lps22hb-pid/lps22hb_reg.c
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sensors)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/logging)