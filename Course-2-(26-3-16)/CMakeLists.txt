cmake_minimum_required(VERSION 3.15)
project(rs232c_homework LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(rs232c_demo
    src/main.cpp
    src/rs232c.cpp
)

target_include_directories(rs232c_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
