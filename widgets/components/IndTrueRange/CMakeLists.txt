cmake_minimum_required(VERSION 3.20)

project(IndTrueRange LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndTrueRange main_IndTrueRange.cpp)

target_link_libraries(IndTrueRange PRIVATE X11::X11)
