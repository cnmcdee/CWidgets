cmake_minimum_required(VERSION 3.20)

project(GtkScaleButton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkScaleButton main_GtkScaleButton.cpp)

target_link_libraries(GtkScaleButton PRIVATE X11::X11)
