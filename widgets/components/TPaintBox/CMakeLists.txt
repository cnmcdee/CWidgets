cmake_minimum_required(VERSION 3.20)

project(TPaintBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TPaintBox main_TPaintBox.cpp)

target_link_libraries(TPaintBox PRIVATE X11::X11)
