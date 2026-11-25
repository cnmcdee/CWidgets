cmake_minimum_required(VERSION 3.20)

project(IndInertia LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndInertia main_IndInertia.cpp)

target_link_libraries(IndInertia PRIVATE X11::X11)
