cmake_minimum_required(VERSION 3.20)

project(IndUpDownVolumeRatio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndUpDownVolumeRatio main_IndUpDownVolumeRatio.cpp)

target_link_libraries(IndUpDownVolumeRatio PRIVATE X11::X11)
