cmake_minimum_required(VERSION 3.20)

project(InstToggleSwitch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstToggleSwitch main_InstToggleSwitch.cpp)

target_link_libraries(InstToggleSwitch PRIVATE X11::X11)
