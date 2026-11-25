cmake_minimum_required(VERSION 3.20)

project(GtkProgressBar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkProgressBar main_GtkProgressBar.cpp)

target_link_libraries(GtkProgressBar PRIVATE X11::X11)
