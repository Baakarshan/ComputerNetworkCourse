cmake_minimum_required(VERSION 3.15)
project(course3_homework LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(course3_demo
    src/main.cpp
    src/multiplex.cpp
    src/modulation.cpp
)

target_include_directories(course3_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
