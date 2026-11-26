cmake_minimum_required(VERSION 3.20)

project(InstVerticalTankLevel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstVerticalTankLevel main_InstVerticalTankLevel.cpp)

target_link_libraries(InstVerticalTankLevel PRIVATE X11::X11)
