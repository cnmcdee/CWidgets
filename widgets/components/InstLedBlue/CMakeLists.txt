cmake_minimum_required(VERSION 3.20)

project(InstLedBlue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstLedBlue main_InstLedBlue.cpp)

target_link_libraries(InstLedBlue PRIVATE X11::X11)
