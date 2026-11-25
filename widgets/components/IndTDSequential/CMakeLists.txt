cmake_minimum_required(VERSION 3.20)

project(IndTDSequential LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndTDSequential main_IndTDSequential.cpp)

target_link_libraries(IndTDSequential PRIVATE X11::X11)
