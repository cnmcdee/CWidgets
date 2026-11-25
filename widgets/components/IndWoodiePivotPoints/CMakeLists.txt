cmake_minimum_required(VERSION 3.20)

project(IndWoodiePivotPoints LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndWoodiePivotPoints main_IndWoodiePivotPoints.cpp)

target_link_libraries(IndWoodiePivotPoints PRIVATE X11::X11)
