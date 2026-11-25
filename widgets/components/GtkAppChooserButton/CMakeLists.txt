cmake_minimum_required(VERSION 3.20)

project(GtkAppChooserButton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkAppChooserButton main_GtkAppChooserButton.cpp)

target_link_libraries(GtkAppChooserButton PRIVATE X11::X11)
