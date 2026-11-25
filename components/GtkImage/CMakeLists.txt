cmake_minimum_required(VERSION 3.20)

project(GtkImage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkImage main_GtkImage.cpp)

target_link_libraries(GtkImage PRIVATE X11::X11)
