cmake_minimum_required(VERSION 3.20)

project(GtkAppChooserDialog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkAppChooserDialog main_GtkAppChooserDialog.cpp)

target_link_libraries(GtkAppChooserDialog PRIVATE X11::X11)
