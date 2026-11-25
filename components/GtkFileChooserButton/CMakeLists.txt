cmake_minimum_required(VERSION 3.20)

project(GtkFileChooserButton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkFileChooserButton main_GtkFileChooserButton.cpp)

target_link_libraries(GtkFileChooserButton PRIVATE X11::X11)
