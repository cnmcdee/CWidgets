cmake_minimum_required(VERSION 3.20)

project(IndKST LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndKST main_IndKST.cpp)

target_link_libraries(IndKST PRIVATE X11::X11)
