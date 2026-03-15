cmake_minimum_required(VERSION 3.15)
project(light_communication_homework LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)

add_executable(light_comm
    src/main.cpp
    src/light_comm.cpp
)

target_include_directories(light_comm PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(light_comm PRIVATE ${OpenCV_LIBS})
