cmake_minimum_required(VERSION 3.20)

project(IndRSX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndRSX main_IndRSX.cpp)

target_link_libraries(IndRSX PRIVATE X11::X11)
