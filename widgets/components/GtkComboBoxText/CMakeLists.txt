cmake_minimum_required(VERSION 3.20)

project(GtkComboBoxText LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkComboBoxText main_GtkComboBoxText.cpp)

target_link_libraries(GtkComboBoxText PRIVATE X11::X11)
