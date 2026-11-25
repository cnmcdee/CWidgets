cmake_minimum_required(VERSION 3.20)

project(IndRelativeVolume LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndRelativeVolume main_IndRelativeVolume.cpp)

target_link_libraries(IndRelativeVolume PRIVATE X11::X11)
