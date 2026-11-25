cmake_minimum_required(VERSION 3.20)

project(StockBaselineChart LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(StockBaselineChart main_StockBaselineChart.cpp)

target_link_libraries(StockBaselineChart PRIVATE X11::X11)
