cmake_minimum_required(VERSION 3.20)

project(IndIntradayIntensityIndex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndIntradayIntensityIndex main_IndIntradayIntensityIndex.cpp)

target_link_libraries(IndIntradayIntensityIndex PRIVATE X11::X11)
