cmake_minimum_required(VERSION 3.20)

project(StockHollowCandles LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(StockHollowCandles main_StockHollowCandles.cpp)

target_link_libraries(StockHollowCandles PRIVATE X11::X11)
