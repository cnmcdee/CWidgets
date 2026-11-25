cmake_minimum_required(VERSION 3.20)

project(GtkListBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkListBox main_GtkListBox.cpp)

target_link_libraries(GtkListBox PRIVATE X11::X11)
