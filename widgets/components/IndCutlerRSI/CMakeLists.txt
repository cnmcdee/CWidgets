cmake_minimum_required(VERSION 3.20)

project(IndCutlerRSI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndCutlerRSI main_IndCutlerRSI.cpp)

target_link_libraries(IndCutlerRSI PRIVATE X11::X11)
