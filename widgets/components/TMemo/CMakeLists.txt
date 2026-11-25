cmake_minimum_required(VERSION 3.20)

project(TMemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TMemo main_TMemo.cpp)

target_link_libraries(TMemo PRIVATE X11::X11)
