cmake_minimum_required(VERSION 3.20)

project(TCalcEdit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TCalcEdit main_TCalcEdit.cpp)

target_link_libraries(TCalcEdit PRIVATE X11::X11)
