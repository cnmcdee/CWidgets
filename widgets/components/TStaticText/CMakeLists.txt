cmake_minimum_required(VERSION 3.20)

project(TStaticText LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TStaticText main_TStaticText.cpp)

target_link_libraries(TStaticText PRIVATE X11::X11)
