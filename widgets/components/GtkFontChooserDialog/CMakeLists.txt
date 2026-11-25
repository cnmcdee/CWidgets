cmake_minimum_required(VERSION 3.20)

project(GtkFontChooserDialog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkFontChooserDialog main_GtkFontChooserDialog.cpp)

target_link_libraries(GtkFontChooserDialog PRIVATE X11::X11)
