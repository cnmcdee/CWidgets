cmake_minimum_required(VERSION 3.20)

project(IndVolumeZoneOscillator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndVolumeZoneOscillator main_IndVolumeZoneOscillator.cpp)

target_link_libraries(IndVolumeZoneOscillator PRIVATE X11::X11)
