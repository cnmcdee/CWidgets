cmake_minimum_required(VERSION 3.20)

project(GtkExpander LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkExpander main_GtkExpander.cpp)

target_link_libraries(GtkExpander PRIVATE X11::X11)
