cmake_minimum_required(VERSION 3.20)

project(TSplitter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TSplitter main_TSplitter.cpp)

target_link_libraries(TSplitter PRIVATE X11::X11)
