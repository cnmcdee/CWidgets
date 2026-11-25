cmake_minimum_required(VERSION 3.20)

project(TTabControl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TTabControl main_TTabControl.cpp)

target_link_libraries(TTabControl PRIVATE X11::X11)
