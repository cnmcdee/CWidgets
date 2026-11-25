cmake_minimum_required(VERSION 3.20)

project(GtkFixed LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkFixed main_GtkFixed.cpp)

target_link_libraries(GtkFixed PRIVATE X11::X11)
