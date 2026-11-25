cmake_minimum_required(VERSION 3.20)

project(IndStochasticFull LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndStochasticFull main_IndStochasticFull.cpp)

target_link_libraries(IndStochasticFull PRIVATE X11::X11)
