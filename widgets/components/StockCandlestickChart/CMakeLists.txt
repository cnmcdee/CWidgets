cmake_minimum_required(VERSION 3.20)

project(StockCandlestickChart LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(StockCandlestickChart main_StockCandlestickChart.cpp)

target_link_libraries(StockCandlestickChart PRIVATE X11::X11)
