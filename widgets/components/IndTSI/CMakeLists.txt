cmake_minimum_required(VERSION 3.20)

project(IndTSI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndTSI main_IndTSI.cpp)

target_link_libraries(IndTSI PRIVATE X11::X11)
