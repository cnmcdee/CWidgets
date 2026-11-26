cmake_minimum_required(VERSION 3.20)

project(InstSeg14Display1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstSeg14Display1 main_InstSeg14Display1.cpp)

target_link_libraries(InstSeg14Display1 PRIVATE X11::X11)
