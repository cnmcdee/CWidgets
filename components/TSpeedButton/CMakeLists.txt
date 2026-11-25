cmake_minimum_required(VERSION 3.20)

project(TSpeedButton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TSpeedButton main_TSpeedButton.cpp)

target_link_libraries(TSpeedButton PRIVATE X11::X11)
