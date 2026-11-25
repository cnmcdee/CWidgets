cmake_minimum_required(VERSION 3.20)

project(TApplicationProperties LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TApplicationProperties main_TApplicationProperties.cpp)

target_link_libraries(TApplicationProperties PRIVATE X11::X11)
