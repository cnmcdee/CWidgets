cmake_minimum_required(VERSION 3.20)

project(GtkInfoBar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkInfoBar main_GtkInfoBar.cpp)

target_link_libraries(GtkInfoBar PRIVATE X11::X11)
