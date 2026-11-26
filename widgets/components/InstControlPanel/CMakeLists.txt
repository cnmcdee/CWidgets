cmake_minimum_required(VERSION 3.20)

project(InstControlPanel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(InstControlPanel main_InstControlPanel.cpp)

target_link_libraries(InstControlPanel PRIVATE X11::X11)
