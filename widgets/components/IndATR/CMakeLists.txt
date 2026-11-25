cmake_minimum_required(VERSION 3.20)

project(IndATR LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndATR main_IndATR.cpp)

target_link_libraries(IndATR PRIVATE X11::X11)
