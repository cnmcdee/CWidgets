cmake_minimum_required(VERSION 3.20)

project(TComboBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)

add_executable(TComboBox main_TComboBox.cpp)

target_link_libraries(TComboBox PRIVATE X11::X11)
