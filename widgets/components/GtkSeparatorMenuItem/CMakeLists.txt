cmake_minimum_required(VERSION 3.20)

project(GtkSeparatorMenuItem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkSeparatorMenuItem main_GtkSeparatorMenuItem.cpp)

target_link_libraries(GtkSeparatorMenuItem PRIVATE X11::X11)
