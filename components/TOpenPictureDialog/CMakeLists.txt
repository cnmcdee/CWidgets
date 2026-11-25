cmake_minimum_required(VERSION 3.20)

project(TOpenPictureDialog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TOpenPictureDialog main_TOpenPictureDialog.cpp)

target_link_libraries(TOpenPictureDialog PRIVATE X11::X11)
