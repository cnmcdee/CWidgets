cmake_minimum_required(VERSION 3.20)

project(TImageList LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TImageList main_TImageList.cpp)

target_link_libraries(TImageList PRIVATE X11::X11)
