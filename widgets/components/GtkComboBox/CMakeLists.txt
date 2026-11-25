cmake_minimum_required(VERSION 3.20)

project(GtkComboBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkComboBox main_GtkComboBox.cpp)

target_link_libraries(GtkComboBox PRIVATE X11::X11)
