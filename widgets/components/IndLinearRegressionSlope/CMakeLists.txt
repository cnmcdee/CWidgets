cmake_minimum_required(VERSION 3.20)

project(IndLinearRegressionSlope LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndLinearRegressionSlope main_IndLinearRegressionSlope.cpp)

target_link_libraries(IndLinearRegressionSlope PRIVATE X11::X11)
