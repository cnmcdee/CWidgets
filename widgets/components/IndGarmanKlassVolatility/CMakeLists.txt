cmake_minimum_required(VERSION 3.20)

project(IndGarmanKlassVolatility LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndGarmanKlassVolatility main_IndGarmanKlassVolatility.cpp)

target_link_libraries(IndGarmanKlassVolatility PRIVATE X11::X11)
