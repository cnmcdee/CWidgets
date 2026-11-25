cmake_minimum_required(VERSION 3.20)

project(GtkToolButton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkToolButton main_GtkToolButton.cpp)

target_link_libraries(GtkToolButton PRIVATE X11::X11)
