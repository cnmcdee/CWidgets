cmake_minimum_required(VERSION 3.20)

project(GtkLockButton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(GtkLockButton main_GtkLockButton.cpp)

target_link_libraries(GtkLockButton PRIVATE X11::X11)
