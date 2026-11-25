cmake_minimum_required(VERSION 3.20)

project(TEditButton LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TEditButton main_TEditButton.cpp)

target_link_libraries(TEditButton PRIVATE X11::X11)
