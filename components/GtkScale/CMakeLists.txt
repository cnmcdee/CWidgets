cmake_minimum_required(VERSION 3.20)

project(GtkScale LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkScale main_GtkScale.cpp)

target_link_libraries(GtkScale PRIVATE X11::X11)
