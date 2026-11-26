cmake_minimum_required(VERSION 3.20)

project(InstBarMeter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstBarMeter main_InstBarMeter.cpp)

target_link_libraries(InstBarMeter PRIVATE X11::X11)
