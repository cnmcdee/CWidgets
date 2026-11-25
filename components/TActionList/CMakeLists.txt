cmake_minimum_required(VERSION 3.20)

project(TActionList LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TActionList main_TActionList.cpp)

target_link_libraries(TActionList PRIVATE X11::X11)
