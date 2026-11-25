cmake_minimum_required(VERSION 3.20)

project(TArrow LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TArrow main_TArrow.cpp)

target_link_libraries(TArrow PRIVATE X11::X11)
