cmake_minimum_required(VERSION 3.20)

project(TButton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TButton main_TButton.cpp)

target_link_libraries(TButton PRIVATE X11::X11)
