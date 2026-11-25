cmake_minimum_required(VERSION 3.20)

project(IndRoofingFilter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndRoofingFilter main_IndRoofingFilter.cpp)

target_link_libraries(IndRoofingFilter PRIVATE X11::X11)
