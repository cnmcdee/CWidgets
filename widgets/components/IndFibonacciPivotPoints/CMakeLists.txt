cmake_minimum_required(VERSION 3.20)

project(IndFibonacciPivotPoints LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndFibonacciPivotPoints main_IndFibonacciPivotPoints.cpp)

target_link_libraries(IndFibonacciPivotPoints PRIVATE X11::X11)
