cmake_minimum_required(VERSION 3.20)

project(IndCenterOfGravity LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(IndCenterOfGravity main_IndCenterOfGravity.cpp)

target_link_libraries(IndCenterOfGravity PRIVATE X11::X11)
