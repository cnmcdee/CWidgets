cmake_minimum_required(VERSION 3.20)

project(StockLineChart LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(StockLineChart main_StockLineChart.cpp)

target_link_libraries(StockLineChart PRIVATE X11::X11)
