cmake_minimum_required(VERSION 3.20)

project(IndChoppinessIndex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndChoppinessIndex main_IndChoppinessIndex.cpp)

target_link_libraries(IndChoppinessIndex PRIVATE X11::X11)
