cmake_minimum_required(VERSION 3.20)

project(IndChaikinVolatility LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndChaikinVolatility main_IndChaikinVolatility.cpp)

target_link_libraries(IndChaikinVolatility PRIVATE X11::X11)
