cmake_minimum_required(VERSION 3.20)

project(IndCloseVolatility LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndCloseVolatility main_IndCloseVolatility.cpp)

target_link_libraries(IndCloseVolatility PRIVATE X11::X11)
