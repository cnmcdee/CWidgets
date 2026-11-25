cmake_minimum_required(VERSION 3.20)

project(TImage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TImage main_TImage.cpp)

target_link_libraries(TImage PRIVATE X11::X11)
