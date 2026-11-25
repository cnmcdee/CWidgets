cmake_minimum_required(VERSION 3.20)

project(IndVolumeAverage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndVolumeAverage main_IndVolumeAverage.cpp)

target_link_libraries(IndVolumeAverage PRIVATE X11::X11)
