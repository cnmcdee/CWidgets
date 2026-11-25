cmake_minimum_required(VERSION 3.20)

project(IndBearPower LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndBearPower main_IndBearPower.cpp)

target_link_libraries(IndBearPower PRIVATE X11::X11)
