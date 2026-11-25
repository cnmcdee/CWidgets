cmake_minimum_required(VERSION 3.20)

project(TSavePictureDialog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TSavePictureDialog main_TSavePictureDialog.cpp)

target_link_libraries(TSavePictureDialog PRIVATE X11::X11)
