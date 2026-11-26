cmake_minimum_required(VERSION 3.20)

project(InstStackLight LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstStackLight main_InstStackLight.cpp)

target_link_libraries(InstStackLight PRIVATE X11::X11)
