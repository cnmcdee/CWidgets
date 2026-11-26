cmake_minimum_required(VERSION 3.20)

project(InstNeedleKnifeEdge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstNeedleKnifeEdge main_InstNeedleKnifeEdge.cpp)

target_link_libraries(InstNeedleKnifeEdge PRIVATE X11::X11)
