cmake_minimum_required(VERSION 3.20)

project(TToggleBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TToggleBox main_TToggleBox.cpp)

target_link_libraries(TToggleBox PRIVATE X11::X11)
