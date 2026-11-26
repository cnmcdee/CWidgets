cmake_minimum_required(VERSION 3.20)

project(InstRgbStatusLed LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstRgbStatusLed main_InstRgbStatusLed.cpp)

target_link_libraries(InstRgbStatusLed PRIVATE X11::X11)
