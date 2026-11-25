cmake_minimum_required(VERSION 3.20)

project(GtkDialog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkDialog main_GtkDialog.cpp)

target_link_libraries(GtkDialog PRIVATE X11::X11)
