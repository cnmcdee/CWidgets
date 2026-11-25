cmake_minimum_required(VERSION 3.20)

project(TPanel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TPanel main_TPanel.cpp)

target_link_libraries(TPanel PRIVATE X11::X11)
