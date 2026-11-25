cmake_minimum_required(VERSION 3.20)

project(IndVolumeRateOfChange LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndVolumeRateOfChange main_IndVolumeRateOfChange.cpp)

target_link_libraries(IndVolumeRateOfChange PRIVATE X11::X11)
