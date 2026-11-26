cmake_minimum_required(VERSION 3.20)

project(InstSeg14Display5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstSeg14Display5 main_InstSeg14Display5.cpp)

target_link_libraries(InstSeg14Display5 PRIVATE X11::X11)
