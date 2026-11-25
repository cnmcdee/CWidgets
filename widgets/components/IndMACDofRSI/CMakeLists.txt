cmake_minimum_required(VERSION 3.20)

project(IndMACDofRSI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndMACDofRSI main_IndMACDofRSI.cpp)

target_link_libraries(IndMACDofRSI PRIVATE X11::X11)
