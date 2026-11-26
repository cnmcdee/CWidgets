cmake_minimum_required(VERSION 3.20)

project(InstTemperatureGauge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstTemperatureGauge main_InstTemperatureGauge.cpp)

target_link_libraries(InstTemperatureGauge PRIVATE X11::X11)
