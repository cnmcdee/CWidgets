cmake_minimum_required(VERSION 3.20)

project(XGraph LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(XGraph main_XGraph.cpp)

target_link_libraries(XGraph PRIVATE X11::X11)
