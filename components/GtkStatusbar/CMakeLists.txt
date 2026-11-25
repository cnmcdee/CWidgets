cmake_minimum_required(VERSION 3.20)

project(GtkStatusbar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkStatusbar main_GtkStatusbar.cpp)

target_link_libraries(GtkStatusbar PRIVATE X11::X11)
