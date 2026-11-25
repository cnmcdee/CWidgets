cmake_minimum_required(VERSION 3.20)

project(IndVolatilityRatio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndVolatilityRatio main_IndVolatilityRatio.cpp)

target_link_libraries(IndVolatilityRatio PRIVATE X11::X11)
