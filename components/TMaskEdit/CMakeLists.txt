cmake_minimum_required(VERSION 3.20)

project(TMaskEdit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TMaskEdit main_TMaskEdit.cpp)

target_link_libraries(TMaskEdit PRIVATE X11::X11)
