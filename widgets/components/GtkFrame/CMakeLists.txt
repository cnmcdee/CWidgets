cmake_minimum_required(VERSION 3.20)

project(GtkFrame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkFrame main_GtkFrame.cpp)

target_link_libraries(GtkFrame PRIVATE X11::X11)
