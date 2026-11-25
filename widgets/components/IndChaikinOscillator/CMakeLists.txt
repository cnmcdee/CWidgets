cmake_minimum_required(VERSION 3.20)

project(IndChaikinOscillator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndChaikinOscillator main_IndChaikinOscillator.cpp)

target_link_libraries(IndChaikinOscillator PRIVATE X11::X11)
