cmake_minimum_required(VERSION 3.20)

project(TColorBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TColorBox main_TColorBox.cpp)

target_link_libraries(TColorBox PRIVATE X11::X11)
