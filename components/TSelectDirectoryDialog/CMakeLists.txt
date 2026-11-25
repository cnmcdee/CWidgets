cmake_minimum_required(VERSION 3.20)

project(TSelectDirectoryDialog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TSelectDirectoryDialog main_TSelectDirectoryDialog.cpp)

target_link_libraries(TSelectDirectoryDialog PRIVATE X11::X11)
