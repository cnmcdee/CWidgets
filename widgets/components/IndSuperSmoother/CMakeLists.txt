cmake_minimum_required(VERSION 3.20)

project(IndSuperSmoother LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndSuperSmoother main_IndSuperSmoother.cpp)

target_link_libraries(IndSuperSmoother PRIVATE X11::X11)
