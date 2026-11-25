cmake_minimum_required(VERSION 3.20)

project(GtkPasswordEntry LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkPasswordEntry main_GtkPasswordEntry.cpp)

target_link_libraries(GtkPasswordEntry PRIVATE X11::X11)
