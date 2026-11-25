cmake_minimum_required(VERSION 3.20)

project(GtkMenu LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkMenu main_GtkMenu.cpp)

target_link_libraries(GtkMenu PRIVATE X11::X11)
