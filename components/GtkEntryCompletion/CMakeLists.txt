cmake_minimum_required(VERSION 3.20)

project(GtkEntryCompletion LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkEntryCompletion main_GtkEntryCompletion.cpp)

target_link_libraries(GtkEntryCompletion PRIVATE X11::X11)
