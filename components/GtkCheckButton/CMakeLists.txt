cmake_minimum_required(VERSION 3.20)

project(GtkCheckButton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkCheckButton main_GtkCheckButton.cpp)

target_link_libraries(GtkCheckButton PRIVATE X11::X11)
