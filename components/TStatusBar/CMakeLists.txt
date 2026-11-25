cmake_minimum_required(VERSION 3.20)

project(TStatusBar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TStatusBar main_TStatusBar.cpp)

target_link_libraries(TStatusBar PRIVATE X11::X11)
