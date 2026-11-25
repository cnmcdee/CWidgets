cmake_minimum_required(VERSION 3.20)

project(IndVariance LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndVariance main_IndVariance.cpp)

target_link_libraries(IndVariance PRIVATE X11::X11)
