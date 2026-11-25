cmake_minimum_required(VERSION 3.20)

project(IndUltimateOscillator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndUltimateOscillator main_IndUltimateOscillator.cpp)

target_link_libraries(IndUltimateOscillator PRIVATE X11::X11)
