cmake_minimum_required(VERSION 3.20)

project(InstFlowMeter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstFlowMeter main_InstFlowMeter.cpp)

target_link_libraries(InstFlowMeter PRIVATE X11::X11)
