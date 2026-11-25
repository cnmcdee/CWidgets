cmake_minimum_required(VERSION 3.20)

project(TBitBtn LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TBitBtn main_TBitBtn.cpp)

target_link_libraries(TBitBtn PRIVATE X11::X11)
