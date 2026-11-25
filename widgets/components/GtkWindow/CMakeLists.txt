cmake_minimum_required(VERSION 3.20)

project(GtkWindow LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkWindow main_GtkWindow.cpp)

target_link_libraries(GtkWindow PRIVATE X11::X11)
