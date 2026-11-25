cmake_minimum_required(VERSION 3.20)

project(IndKairi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndKairi main_IndKairi.cpp)

target_link_libraries(IndKairi PRIVATE X11::X11)
