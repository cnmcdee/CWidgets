cmake_minimum_required(VERSION 3.20)

project(TGroupBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TGroupBox main_TGroupBox.cpp)

target_link_libraries(TGroupBox PRIVATE X11::X11)
