cmake_minimum_required(VERSION 3.20)

project(TTimer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TTimer main_TTimer.cpp)

target_link_libraries(TTimer PRIVATE X11::X11)
