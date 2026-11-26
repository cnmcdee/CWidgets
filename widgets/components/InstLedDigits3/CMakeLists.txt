cmake_minimum_required(VERSION 3.20)

project(InstLedDigits3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstLedDigits3 main_InstLedDigits3.cpp)

target_link_libraries(InstLedDigits3 PRIVATE X11::X11)
