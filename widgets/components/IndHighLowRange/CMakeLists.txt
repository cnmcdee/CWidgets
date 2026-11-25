cmake_minimum_required(VERSION 3.20)

project(IndHighLowRange LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndHighLowRange main_IndHighLowRange.cpp)

target_link_libraries(IndHighLowRange PRIVATE X11::X11)
