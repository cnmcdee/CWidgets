cmake_minimum_required(VERSION 3.20)

project(TColorButton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TColorButton main_TColorButton.cpp)

target_link_libraries(TColorButton PRIVATE X11::X11)
