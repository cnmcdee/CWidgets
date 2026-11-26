cmake_minimum_required(VERSION 3.20)

project(InstLevelTransmitter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstLevelTransmitter main_InstLevelTransmitter.cpp)

target_link_libraries(InstLevelTransmitter PRIVATE X11::X11)
