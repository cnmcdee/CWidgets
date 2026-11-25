cmake_minimum_required(VERSION 3.20)

project(IndDominantCycle LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndDominantCycle main_IndDominantCycle.cpp)

target_link_libraries(IndDominantCycle PRIVATE X11::X11)
