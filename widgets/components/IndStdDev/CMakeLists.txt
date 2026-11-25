cmake_minimum_required(VERSION 3.20)

project(IndStdDev LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndStdDev main_IndStdDev.cpp)

target_link_libraries(IndStdDev PRIVATE X11::X11)
