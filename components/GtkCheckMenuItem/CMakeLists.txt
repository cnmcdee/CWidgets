cmake_minimum_required(VERSION 3.20)

project(GtkCheckMenuItem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkCheckMenuItem main_GtkCheckMenuItem.cpp)

target_link_libraries(GtkCheckMenuItem PRIVATE X11::X11)
