cmake_minimum_required(VERSION 3.20)

project(GtkHeaderBar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkHeaderBar main_GtkHeaderBar.cpp)

target_link_libraries(GtkHeaderBar PRIVATE X11::X11)
