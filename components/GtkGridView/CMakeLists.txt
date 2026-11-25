cmake_minimum_required(VERSION 3.20)

project(GtkGridView LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkGridView main_GtkGridView.cpp)

target_link_libraries(GtkGridView PRIVATE X11::X11)
