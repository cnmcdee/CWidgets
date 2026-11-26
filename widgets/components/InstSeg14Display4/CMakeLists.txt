cmake_minimum_required(VERSION 3.20)

project(InstSeg14Display4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstSeg14Display4 main_InstSeg14Display4.cpp)

target_link_libraries(InstSeg14Display4 PRIVATE X11::X11)
