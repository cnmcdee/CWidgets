cmake_minimum_required(VERSION 3.20)

project(GtkToggleToolButton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkToggleToolButton main_GtkToggleToolButton.cpp)

target_link_libraries(GtkToggleToolButton PRIVATE X11::X11)
