cmake_minimum_required(VERSION 3.20)

project(IndRSquared LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndRSquared main_IndRSquared.cpp)

target_link_libraries(IndRSquared PRIVATE X11::X11)
