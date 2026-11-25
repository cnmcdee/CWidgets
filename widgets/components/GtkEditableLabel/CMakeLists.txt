cmake_minimum_required(VERSION 3.20)

project(GtkEditableLabel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkEditableLabel main_GtkEditableLabel.cpp)

target_link_libraries(GtkEditableLabel PRIVATE X11::X11)
