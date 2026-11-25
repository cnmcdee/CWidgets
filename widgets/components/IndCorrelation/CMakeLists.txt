cmake_minimum_required(VERSION 3.20)

project(IndCorrelation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndCorrelation main_IndCorrelation.cpp)

target_link_libraries(IndCorrelation PRIVATE X11::X11)
