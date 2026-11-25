cmake_minimum_required(VERSION 3.20)

project(GtkListView LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkListView main_GtkListView.cpp)

target_link_libraries(GtkListView PRIVATE X11::X11)
