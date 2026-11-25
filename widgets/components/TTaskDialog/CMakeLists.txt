cmake_minimum_required(VERSION 3.20)

project(TTaskDialog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TTaskDialog main_TTaskDialog.cpp)

target_link_libraries(TTaskDialog PRIVATE X11::X11)
