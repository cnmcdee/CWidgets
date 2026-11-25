cmake_minimum_required(VERSION 3.20)

project(IndLaguerreFilter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndLaguerreFilter main_IndLaguerreFilter.cpp)

target_link_libraries(IndLaguerreFilter PRIVATE X11::X11)
