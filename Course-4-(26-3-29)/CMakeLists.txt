cmake_minimum_required(VERSION 3.15)
project(course4_homework LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(course4_demo
    src/main.cpp
    src/parity_check.cpp
)

target_include_directories(course4_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
