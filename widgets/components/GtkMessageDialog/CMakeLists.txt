cmake_minimum_required(VERSION 3.20)

project(GtkMessageDialog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkMessageDialog main_GtkMessageDialog.cpp)

target_link_libraries(GtkMessageDialog PRIVATE X11::X11)
