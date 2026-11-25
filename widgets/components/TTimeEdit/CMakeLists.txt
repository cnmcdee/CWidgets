cmake_minimum_required(VERSION 3.20)

project(TTimeEdit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TTimeEdit main_TTimeEdit.cpp)

target_link_libraries(TTimeEdit PRIVATE X11::X11)
