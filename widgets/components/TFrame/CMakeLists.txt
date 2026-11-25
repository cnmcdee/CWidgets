cmake_minimum_required(VERSION 3.20)

project(TFrame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TFrame main_TFrame.cpp)

target_link_libraries(TFrame PRIVATE X11::X11)
