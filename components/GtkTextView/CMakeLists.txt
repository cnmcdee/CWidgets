cmake_minimum_required(VERSION 3.20)

project(GtkTextView LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkTextView main_GtkTextView.cpp)

target_link_libraries(GtkTextView PRIVATE X11::X11)
