cmake_minimum_required(VERSION 3.20)

project(IndUlcerIndex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndUlcerIndex main_IndUlcerIndex.cpp)

target_link_libraries(IndUlcerIndex PRIVATE X11::X11)
