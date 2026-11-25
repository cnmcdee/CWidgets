cmake_minimum_required(VERSION 3.20)

project(GtkPaned LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkPaned main_GtkPaned.cpp)

target_link_libraries(GtkPaned PRIVATE X11::X11)
