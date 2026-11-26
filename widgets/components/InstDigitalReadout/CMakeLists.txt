cmake_minimum_required(VERSION 3.20)

project(InstDigitalReadout LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstDigitalReadout main_InstDigitalReadout.cpp)

target_link_libraries(InstDigitalReadout PRIVATE X11::X11)
