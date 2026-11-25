cmake_minimum_required(VERSION 3.20)

project(IndHurstExponent LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndHurstExponent main_IndHurstExponent.cpp)

target_link_libraries(IndHurstExponent PRIVATE X11::X11)
