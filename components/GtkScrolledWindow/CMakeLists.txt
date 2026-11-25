cmake_minimum_required(VERSION 3.20)

project(GtkScrolledWindow LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkScrolledWindow main_GtkScrolledWindow.cpp)

target_link_libraries(GtkScrolledWindow PRIVATE X11::X11)
