cmake_minimum_required(VERSION 3.20)

project(InstLedDigits5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstLedDigits5 main_InstLedDigits5.cpp)

target_link_libraries(InstLedDigits5 PRIVATE X11::X11)
