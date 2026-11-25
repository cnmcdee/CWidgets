cmake_minimum_required(VERSION 3.20)

project(IndRSI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndRSI main_IndRSI.cpp)

target_link_libraries(IndRSI PRIVATE X11::X11)
