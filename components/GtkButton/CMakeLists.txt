cmake_minimum_required(VERSION 3.20)

project(GtkButton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkButton main_GtkButton.cpp)

target_link_libraries(GtkButton PRIVATE X11::X11)
