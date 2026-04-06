cmake_minimum_required(VERSION 3.15)
project(course5_homework LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(course5_demo
    src/main.cpp
    src/ethernet.cpp
    src/csma_cd.cpp
)

target_include_directories(course5_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
