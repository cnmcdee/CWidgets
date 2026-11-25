cmake_minimum_required(VERSION 3.20)

project(IndAPO LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndAPO main_IndAPO.cpp)

target_link_libraries(IndAPO PRIVATE X11::X11)
