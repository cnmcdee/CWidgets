cmake_minimum_required(VERSION 3.20)

project(GtkInvisible LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkInvisible main_GtkInvisible.cpp)

target_link_libraries(GtkInvisible PRIVATE X11::X11)
