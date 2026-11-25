cmake_minimum_required(VERSION 3.20)

project(StockHeikinAshi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(StockHeikinAshi main_StockHeikinAshi.cpp)

target_link_libraries(StockHeikinAshi PRIVATE X11::X11)
