cmake_minimum_required(VERSION 3.20)

project(GtkPopover LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkPopover main_GtkPopover.cpp)

target_link_libraries(GtkPopover PRIVATE X11::X11)
