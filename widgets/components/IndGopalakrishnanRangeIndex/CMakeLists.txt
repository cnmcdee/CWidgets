cmake_minimum_required(VERSION 3.20)

project(IndGopalakrishnanRangeIndex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndGopalakrishnanRangeIndex main_IndGopalakrishnanRangeIndex.cpp)

target_link_libraries(IndGopalakrishnanRangeIndex PRIVATE X11::X11)
