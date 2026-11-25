cmake_minimum_required(VERSION 3.20)

project(IndWilliamsAccumulationDistribution LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndWilliamsAccumulationDistribution main_IndWilliamsAccumulationDistribution.cpp)

target_link_libraries(IndWilliamsAccumulationDistribution PRIVATE X11::X11)
