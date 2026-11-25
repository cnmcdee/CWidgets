cmake_minimum_required(VERSION 3.20)

project(IndLaguerreRSI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndLaguerreRSI main_IndLaguerreRSI.cpp)

target_link_libraries(IndLaguerreRSI PRIVATE X11::X11)
