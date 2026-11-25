cmake_minimum_required(VERSION 3.20)

project(GtkCellView LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkCellView main_GtkCellView.cpp)

target_link_libraries(GtkCellView PRIVATE X11::X11)
