cmake_minimum_required(VERSION 3.20)

project(GtkToolbar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkToolbar main_GtkToolbar.cpp)

target_link_libraries(GtkToolbar PRIVATE X11::X11)
