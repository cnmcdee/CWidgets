cmake_minimum_required(VERSION 3.20)

project(IndVolumeWeightedMovingAverage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndVolumeWeightedMovingAverage main_IndVolumeWeightedMovingAverage.cpp)

target_link_libraries(IndVolumeWeightedMovingAverage PRIVATE X11::X11)
