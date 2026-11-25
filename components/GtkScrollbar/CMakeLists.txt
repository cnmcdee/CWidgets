cmake_minimum_required(VERSION 3.20)

project(GtkScrollbar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkScrollbar main_GtkScrollbar.cpp)

target_link_libraries(GtkScrollbar PRIVATE X11::X11)
