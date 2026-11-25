cmake_minimum_required(VERSION 3.20)

project(GtkRadioButton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkRadioButton main_GtkRadioButton.cpp)

target_link_libraries(GtkRadioButton PRIVATE X11::X11)
