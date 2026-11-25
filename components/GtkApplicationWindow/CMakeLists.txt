cmake_minimum_required(VERSION 3.20)

project(GtkApplicationWindow LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkApplicationWindow main_GtkApplicationWindow.cpp)

target_link_libraries(GtkApplicationWindow PRIVATE X11::X11)
