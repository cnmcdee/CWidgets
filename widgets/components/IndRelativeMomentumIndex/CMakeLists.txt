cmake_minimum_required(VERSION 3.20)

project(IndRelativeMomentumIndex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndRelativeMomentumIndex main_IndRelativeMomentumIndex.cpp)

target_link_libraries(IndRelativeMomentumIndex PRIVATE X11::X11)
