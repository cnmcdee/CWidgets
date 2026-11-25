cmake_minimum_required(VERSION 3.20)

project(GtkSeparator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkSeparator main_GtkSeparator.cpp)

target_link_libraries(GtkSeparator PRIVATE X11::X11)
