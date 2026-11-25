cmake_minimum_required(VERSION 3.20)

project(GtkFileChooserDialog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkFileChooserDialog main_GtkFileChooserDialog.cpp)

target_link_libraries(GtkFileChooserDialog PRIVATE X11::X11)
