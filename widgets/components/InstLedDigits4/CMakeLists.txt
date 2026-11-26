cmake_minimum_required(VERSION 3.20)

project(InstLedDigits4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstLedDigits4 main_InstLedDigits4.cpp)

target_link_libraries(InstLedDigits4 PRIVATE X11::X11)
