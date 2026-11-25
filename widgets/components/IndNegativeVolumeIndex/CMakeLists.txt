cmake_minimum_required(VERSION 3.20)

project(IndNegativeVolumeIndex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndNegativeVolumeIndex main_IndNegativeVolumeIndex.cpp)

target_link_libraries(IndNegativeVolumeIndex PRIVATE X11::X11)
