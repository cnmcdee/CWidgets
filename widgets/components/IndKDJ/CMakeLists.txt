cmake_minimum_required(VERSION 3.20)

project(IndKDJ LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndKDJ main_IndKDJ.cpp)

target_link_libraries(IndKDJ PRIVATE X11::X11)
