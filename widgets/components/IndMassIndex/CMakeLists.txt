cmake_minimum_required(VERSION 3.20)

project(IndMassIndex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndMassIndex main_IndMassIndex.cpp)

target_link_libraries(IndMassIndex PRIVATE X11::X11)
