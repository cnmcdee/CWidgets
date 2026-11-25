cmake_minimum_required(VERSION 3.20)

project(GtkColorButton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkColorButton main_GtkColorButton.cpp)

target_link_libraries(GtkColorButton PRIVATE X11::X11)
