cmake_minimum_required(VERSION 3.20)

project(IndAdaptiveCyberCycle LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndAdaptiveCyberCycle main_IndAdaptiveCyberCycle.cpp)

target_link_libraries(IndAdaptiveCyberCycle PRIVATE X11::X11)
