cmake_minimum_required(VERSION 3.20)

project(InstRotarySelector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstRotarySelector main_InstRotarySelector.cpp)

target_link_libraries(InstRotarySelector PRIVATE X11::X11)
