cmake_minimum_required(VERSION 3.20)

project(GtkButtonBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkButtonBox main_GtkButtonBox.cpp)

target_link_libraries(GtkButtonBox PRIVATE X11::X11)
