cmake_minimum_required(VERSION 3.20)

project(GtkTreeView LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkTreeView main_GtkTreeView.cpp)

target_link_libraries(GtkTreeView PRIVATE X11::X11)
