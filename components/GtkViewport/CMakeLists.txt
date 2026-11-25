cmake_minimum_required(VERSION 3.20)

project(GtkViewport LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkViewport main_GtkViewport.cpp)

target_link_libraries(GtkViewport PRIVATE X11::X11)
