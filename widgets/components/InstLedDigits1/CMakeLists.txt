cmake_minimum_required(VERSION 3.20)

project(InstLedDigits1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstLedDigits1 main_InstLedDigits1.cpp)

target_link_libraries(InstLedDigits1 PRIVATE X11::X11)
