cmake_minimum_required(VERSION 3.20)

project(TControlBar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TControlBar main_TControlBar.cpp)

target_link_libraries(TControlBar PRIVATE X11::X11)
