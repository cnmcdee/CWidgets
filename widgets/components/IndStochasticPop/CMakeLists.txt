cmake_minimum_required(VERSION 3.20)

project(IndStochasticPop LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndStochasticPop main_IndStochasticPop.cpp)

target_link_libraries(IndStochasticPop PRIVATE X11::X11)
