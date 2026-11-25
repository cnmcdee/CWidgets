cmake_minimum_required(VERSION 3.20)

project(GtkDrawingArea LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkDrawingArea main_GtkDrawingArea.cpp)

target_link_libraries(GtkDrawingArea PRIVATE X11::X11)
