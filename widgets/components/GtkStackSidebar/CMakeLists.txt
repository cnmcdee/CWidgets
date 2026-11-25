cmake_minimum_required(VERSION 3.20)

project(GtkStackSidebar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkStackSidebar main_GtkStackSidebar.cpp)

target_link_libraries(GtkStackSidebar PRIVATE X11::X11)
