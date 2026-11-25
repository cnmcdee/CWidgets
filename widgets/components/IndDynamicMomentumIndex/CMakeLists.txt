cmake_minimum_required(VERSION 3.20)

project(IndDynamicMomentumIndex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndDynamicMomentumIndex main_IndDynamicMomentumIndex.cpp)

target_link_libraries(IndDynamicMomentumIndex PRIVATE X11::X11)
