cmake_minimum_required(VERSION 3.20)

project(IndPercentileRank LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndPercentileRank main_IndPercentileRank.cpp)

target_link_libraries(IndPercentileRank PRIVATE X11::X11)
