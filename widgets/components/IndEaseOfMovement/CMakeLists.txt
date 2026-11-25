cmake_minimum_required(VERSION 3.20)

project(IndEaseOfMovement LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndEaseOfMovement main_IndEaseOfMovement.cpp)

target_link_libraries(IndEaseOfMovement PRIVATE X11::X11)
