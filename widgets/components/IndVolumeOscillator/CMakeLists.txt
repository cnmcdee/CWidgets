cmake_minimum_required(VERSION 3.20)

project(IndVolumeOscillator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndVolumeOscillator main_IndVolumeOscillator.cpp)

target_link_libraries(IndVolumeOscillator PRIVATE X11::X11)
