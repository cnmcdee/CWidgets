cmake_minimum_required(VERSION 3.20)

project(TColorListBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TColorListBox main_TColorListBox.cpp)

target_link_libraries(TColorListBox PRIVATE X11::X11)
