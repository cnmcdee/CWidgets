cmake_minimum_required(VERSION 3.20)

project(IndHistoricalVolatility LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndHistoricalVolatility main_IndHistoricalVolatility.cpp)

target_link_libraries(IndHistoricalVolatility PRIVATE X11::X11)
