cmake_minimum_required(VERSION 3.20)

project(InstSlideSwitch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstSlideSwitch main_InstSlideSwitch.cpp)

target_link_libraries(InstSlideSwitch PRIVATE X11::X11)
