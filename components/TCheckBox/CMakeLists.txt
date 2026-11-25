cmake_minimum_required(VERSION 3.20)

project(TCheckBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TCheckBox main_TCheckBox.cpp)

target_link_libraries(TCheckBox PRIVATE X11::X11)
