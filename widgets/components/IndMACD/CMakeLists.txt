cmake_minimum_required(VERSION 3.20)

project(IndMACD LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndMACD main_IndMACD.cpp)

target_link_libraries(IndMACD PRIVATE X11::X11)
