cmake_minimum_required(VERSION 3.20)

project(THeaderControl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(THeaderControl main_THeaderControl.cpp)

target_link_libraries(THeaderControl PRIVATE X11::X11)
