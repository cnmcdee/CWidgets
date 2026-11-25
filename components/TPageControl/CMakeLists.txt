cmake_minimum_required(VERSION 3.20)

project(TPageControl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TPageControl main_TPageControl.cpp)

target_link_libraries(TPageControl PRIVATE X11::X11)
