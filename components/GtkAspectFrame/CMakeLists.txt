cmake_minimum_required(VERSION 3.20)

project(GtkAspectFrame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkAspectFrame main_GtkAspectFrame.cpp)

target_link_libraries(GtkAspectFrame PRIVATE X11::X11)
