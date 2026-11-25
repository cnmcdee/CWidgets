cmake_minimum_required(VERSION 3.20)

project(IndWeightedClose LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndWeightedClose main_IndWeightedClose.cpp)

target_link_libraries(IndWeightedClose PRIVATE X11::X11)
