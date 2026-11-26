cmake_minimum_required(VERSION 3.20)

project(InstRockerSwitch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstRockerSwitch main_InstRockerSwitch.cpp)

target_link_libraries(InstRockerSwitch PRIVATE X11::X11)
