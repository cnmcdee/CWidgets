cmake_minimum_required(VERSION 3.20)

project(IndRainbowOscillator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndRainbowOscillator main_IndRainbowOscillator.cpp)

target_link_libraries(IndRainbowOscillator PRIVATE X11::X11)
