cmake_minimum_required(VERSION 3.20)

project(IndSkewness LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndSkewness main_IndSkewness.cpp)

target_link_libraries(IndSkewness PRIVATE X11::X11)
