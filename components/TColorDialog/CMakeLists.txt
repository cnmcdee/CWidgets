cmake_minimum_required(VERSION 3.20)

project(TColorDialog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TColorDialog main_TColorDialog.cpp)

target_link_libraries(TColorDialog PRIVATE X11::X11)
