cmake_minimum_required(VERSION 3.20)

project(GtkSearchBar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkSearchBar main_GtkSearchBar.cpp)

target_link_libraries(GtkSearchBar PRIVATE X11::X11)
