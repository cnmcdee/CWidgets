cmake_minimum_required(VERSION 3.20)

project(IndChaikinMoneyFlow LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndChaikinMoneyFlow main_IndChaikinMoneyFlow.cpp)

target_link_libraries(IndChaikinMoneyFlow PRIVATE X11::X11)
