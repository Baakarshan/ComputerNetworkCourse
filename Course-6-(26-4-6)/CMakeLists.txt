cmake_minimum_required(VERSION 3.15)
project(course6_homework LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(course6_demo
    src/main.cpp
    src/bridge.cpp
    src/ap_deployment.cpp
)

target_include_directories(course6_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
