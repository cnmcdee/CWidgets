cmake_minimum_required(VERSION 3.20)

project(IndSineWave LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndSineWave main_IndSineWave.cpp)

target_link_libraries(IndSineWave PRIVATE X11::X11)
