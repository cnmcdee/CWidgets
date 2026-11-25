cmake_minimum_required(VERSION 3.20)

project(IndMomentum LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndMomentum main_IndMomentum.cpp)

target_link_libraries(IndMomentum PRIVATE X11::X11)
