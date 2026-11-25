cmake_minimum_required(VERSION 3.20)

project(TScrollBar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TScrollBar main_TScrollBar.cpp)

target_link_libraries(TScrollBar PRIVATE X11::X11)
