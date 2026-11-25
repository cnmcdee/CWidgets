cmake_minimum_required(VERSION 3.20)

project(TDrawGrid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TDrawGrid main_TDrawGrid.cpp)

target_link_libraries(TDrawGrid PRIVATE X11::X11)
