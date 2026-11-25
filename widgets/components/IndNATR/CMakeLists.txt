cmake_minimum_required(VERSION 3.20)

project(IndNATR LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndNATR main_IndNATR.cpp)

target_link_libraries(IndNATR PRIVATE X11::X11)
