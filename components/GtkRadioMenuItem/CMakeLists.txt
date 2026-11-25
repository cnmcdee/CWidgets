cmake_minimum_required(VERSION 3.20)

project(GtkRadioMenuItem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkRadioMenuItem main_GtkRadioMenuItem.cpp)

target_link_libraries(GtkRadioMenuItem PRIVATE X11::X11)
