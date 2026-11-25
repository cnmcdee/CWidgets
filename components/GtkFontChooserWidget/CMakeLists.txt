cmake_minimum_required(VERSION 3.20)

project(GtkFontChooserWidget LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkFontChooserWidget main_GtkFontChooserWidget.cpp)

target_link_libraries(GtkFontChooserWidget PRIVATE X11::X11)
