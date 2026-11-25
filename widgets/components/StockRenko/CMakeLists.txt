cmake_minimum_required(VERSION 3.20)

project(StockRenko LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(StockRenko main_StockRenko.cpp)

target_link_libraries(StockRenko PRIVATE X11::X11)
