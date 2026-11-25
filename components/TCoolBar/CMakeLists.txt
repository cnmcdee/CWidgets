cmake_minimum_required(VERSION 3.20)

project(TCoolBar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TCoolBar main_TCoolBar.cpp)

target_link_libraries(TCoolBar PRIVATE X11::X11)
