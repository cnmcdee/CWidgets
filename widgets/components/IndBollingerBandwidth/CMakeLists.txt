cmake_minimum_required(VERSION 3.20)

project(IndBollingerBandwidth LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndBollingerBandwidth main_IndBollingerBandwidth.cpp)

target_link_libraries(IndBollingerBandwidth PRIVATE X11::X11)
