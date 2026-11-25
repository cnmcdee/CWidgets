cmake_minimum_required(VERSION 3.20)

project(IndRangeExpansionIndex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndRangeExpansionIndex main_IndRangeExpansionIndex.cpp)

target_link_libraries(IndRangeExpansionIndex PRIVATE X11::X11)
