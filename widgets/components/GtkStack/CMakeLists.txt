cmake_minimum_required(VERSION 3.20)

project(GtkStack LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkStack main_GtkStack.cpp)

target_link_libraries(GtkStack PRIVATE X11::X11)
