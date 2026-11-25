cmake_minimum_required(VERSION 3.20)

project(IndKeltnerBandwidth LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndKeltnerBandwidth main_IndKeltnerBandwidth.cpp)

target_link_libraries(IndKeltnerBandwidth PRIVATE X11::X11)
