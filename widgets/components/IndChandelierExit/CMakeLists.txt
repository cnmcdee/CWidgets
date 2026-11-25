cmake_minimum_required(VERSION 3.20)

project(IndChandelierExit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndChandelierExit main_IndChandelierExit.cpp)

target_link_libraries(IndChandelierExit PRIVATE X11::X11)
