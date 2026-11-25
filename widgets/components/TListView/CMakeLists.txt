cmake_minimum_required(VERSION 3.20)

project(TListView LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TListView main_TListView.cpp)

target_link_libraries(TListView PRIVATE X11::X11)
