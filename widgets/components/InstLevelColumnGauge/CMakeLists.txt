cmake_minimum_required(VERSION 3.20)

project(InstLevelColumnGauge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstLevelColumnGauge main_InstLevelColumnGauge.cpp)

target_link_libraries(InstLevelColumnGauge PRIVATE X11::X11)
