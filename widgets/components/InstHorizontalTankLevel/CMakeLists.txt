cmake_minimum_required(VERSION 3.20)

project(InstHorizontalTankLevel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstHorizontalTankLevel main_InstHorizontalTankLevel.cpp)

target_link_libraries(InstHorizontalTankLevel PRIVATE X11::X11)
