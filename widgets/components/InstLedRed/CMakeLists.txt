cmake_minimum_required(VERSION 3.20)

project(InstLedRed LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstLedRed main_InstLedRed.cpp)

target_link_libraries(InstLedRed PRIVATE X11::X11)
