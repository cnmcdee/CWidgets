cmake_minimum_required(VERSION 3.20)

project(IndSpecialK LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndSpecialK main_IndSpecialK.cpp)

target_link_libraries(IndSpecialK PRIVATE X11::X11)
