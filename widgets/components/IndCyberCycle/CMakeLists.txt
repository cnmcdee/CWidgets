cmake_minimum_required(VERSION 3.20)

project(IndCyberCycle LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndCyberCycle main_IndCyberCycle.cpp)

target_link_libraries(IndCyberCycle PRIVATE X11::X11)
