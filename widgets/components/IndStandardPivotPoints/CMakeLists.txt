cmake_minimum_required(VERSION 3.20)

project(IndStandardPivotPoints LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndStandardPivotPoints main_IndStandardPivotPoints.cpp)

target_link_libraries(IndStandardPivotPoints PRIVATE X11::X11)
