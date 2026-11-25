cmake_minimum_required(VERSION 3.20)

project(TFindDialog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TFindDialog main_TFindDialog.cpp)

target_link_libraries(TFindDialog PRIVATE X11::X11)
