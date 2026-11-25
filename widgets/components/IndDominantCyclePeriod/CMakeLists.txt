cmake_minimum_required(VERSION 3.20)

project(IndDominantCyclePeriod LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndDominantCyclePeriod main_IndDominantCyclePeriod.cpp)

target_link_libraries(IndDominantCyclePeriod PRIVATE X11::X11)
