cmake_minimum_required(VERSION 3.20)

project(IndDoubleStochastic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndDoubleStochastic main_IndDoubleStochastic.cpp)

target_link_libraries(IndDoubleStochastic PRIVATE X11::X11)
