cmake_minimum_required(VERSION 3.20)

project(GtkGrid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkGrid main_GtkGrid.cpp)

target_link_libraries(GtkGrid PRIVATE X11::X11)
