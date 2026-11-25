cmake_minimum_required(VERSION 3.20)

project(IndZeroLagMACD LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndZeroLagMACD main_IndZeroLagMACD.cpp)

target_link_libraries(IndZeroLagMACD PRIVATE X11::X11)
