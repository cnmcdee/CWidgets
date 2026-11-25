cmake_minimum_required(VERSION 3.20)

project(GtkEventBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkEventBox main_GtkEventBox.cpp)

target_link_libraries(GtkEventBox PRIVATE X11::X11)
